#include "../../utils/base64.h"
#include "./mickey2.h"
#include <stdexcept>
#include <bitset>
#include <iostream>

using namespace std;

class Mickey2Cipher {
    static constexpr int STATE_SIZE = 100;
    array<bool, STATE_SIZE> R{}, S{};

    bool clock_R(bool input_bit, bool control_bit_S) {
        bool feedback = R[99] ^ input_bit;
        array<bool, STATE_SIZE> newR{};
        for (int i = STATE_SIZE - 1; i > 0; --i)
            newR[i] = R[i - 1] ^ (control_bit_S ? R[i] : 0);
        newR[0] = feedback;
        R = newR;
        return feedback;
    }

    bool clock_S(bool input_bit, bool control_bit_R) {
        bool feedback = S[99] ^ input_bit;
        array<bool, STATE_SIZE> newS{};
        for (int i = STATE_SIZE - 1; i > 0; --i) {
            bool tmp = S[i - 1] ^ (control_bit_R && S[(i - 1 + 40) % STATE_SIZE]);
            newS[i] = tmp;
        }
        newS[0] = feedback;
        S = newS;
        return feedback;
    }

    void clock_KG(bool mixing, bool input_bit) {
        bool control_R = S[34] ^ R[67];
        bool control_S = R[33] ^ S[67];
        clock_R(input_bit & mixing, control_S);
        clock_S(input_bit & mixing, control_R);
    }

public:
    Mickey2Cipher(const vector<bool>& key, const vector<bool>& iv) {
        R.fill(false);
        S.fill(false);
        for (bool b : iv) clock_KG(true, b);
        for (bool b : key) clock_KG(true, b);
        for (int i = 0; i < 100; ++i) clock_KG(true, false);
    }

    uint8_t get_byte() {
        uint8_t x = 0;
        for (int i = 0; i < 8; ++i) {
            x = (x << 1) | (R[0] ^ S[0]);
            clock_KG(false, false);
        }
        return x;
    }

    string process(const string& input) {
        string output = input;
        for (size_t i = 0; i < input.size(); ++i) {
            uint8_t k = get_byte();
            output[i] ^= k;
        }
        return output;
    }
};

static vector<bool> stringToBits(const string& str, size_t requiredLen) {
    if (str.size() != requiredLen / 8) {
        throw invalid_argument("Неверная длина: ожидалось " + to_string(requiredLen / 8) + " байт");
    }
    vector<bool> bits;
    for (unsigned char c : str) {
        for (int i = 7; i >= 0; --i) {
            bits.push_back((c >> i) & 1);
        }
    }
    return bits;
}

string Mickey2Algorithm::encrypt(const string& text, const string& key) const {
    auto pos = key.find(':');
    if (pos == string::npos) {
        throw invalid_argument("Ключ должен содержать ':' между ключом и IV");
    }
    string keyPart = key.substr(0, pos);
    string ivPart = key.substr(pos + 1);
    auto keyBits = stringToBits(keyPart, 80);
    auto ivBits = stringToBits(ivPart, 80);
    Mickey2Cipher mickey(keyBits, ivBits);
    string raw = mickey.process(text);
    return base64_encode(raw);
}

string Mickey2Algorithm::decrypt(const string& text, const string& key) const {
    auto pos = key.find(':');
    if (pos == string::npos) {
        throw invalid_argument("Ключ должен содержать ':' между ключом и IV");
    }
    string keyPart = key.substr(0, pos);
    string ivPart = key.substr(pos + 1);
    auto keyBits = stringToBits(keyPart, 80);
    auto ivBits = stringToBits(ivPart, 80);
    Mickey2Cipher mickey(keyBits, ivBits);
    string raw = base64_decode(text);
    return mickey.process(raw);
}

string Mickey2Algorithm::getName() const {
    return "mickey2";
}

string Mickey2Algorithm::getDisplayName() const {
    return "Шифр Mickey 2.0";
}

string Mickey2Algorithm::getInputLabel() const {
    return "Ключ (10 байт) и IV (10 байт):";
}

string Mickey2Algorithm::getInputPlaceholder() const {
    return "например: 1234567890:abcdefghij";
}

ValidationResult Mickey2Algorithm::validateInput(const string& input) const {
    auto pos = input.find(':');
    if (pos == string::npos) {
        return ValidationResult(false, "Ключ должен содержать ':' между ключом и IV");
    }
    string keyPart = input.substr(0, pos);
    string ivPart = input.substr(pos + 1);
    if (keyPart.size() != 10) return ValidationResult(false, "Ключ должен быть 10 байт");
    if (ivPart.size() != 10) return ValidationResult(false, "IV должен быть 10 байт");
    return ValidationResult(true);
}