#include "../../utils/base64.h"
#include "./chacha20.h"
#include <vector>
#include <string>
#include <stdexcept>
#include <sstream>
#include <cstdint>
#include <cstring>

using namespace std;

// Реализация алгоритма потокового шифрования ChaCha20
class ChaCha20 {
public:
    // Конструктор с пользовательским "sigma" (должен быть 16 байт)
    ChaCha20(const string& key, const string& sigmaStr, uint64_t nonce = 0, uint32_t counter = 0) {
        if (key.size() != 32) {
            throw invalid_argument("Ключ ChaCha20 должен быть 32 байта (256 бит)");
        }
        if (sigmaStr.size() != 16) {
            throw invalid_argument("Sigma должна быть ровно 16 символов");
        }

        const uint8_t* k = reinterpret_cast<const uint8_t*>(key.data());
        const uint8_t* n = reinterpret_cast<const uint8_t*>(&nonce);
        const uint8_t* sigma = reinterpret_cast<const uint8_t*>(sigmaStr.data());

        memcpy(state, sigma, 16);
        memcpy(state + 4, k, 32);
        state[12] = counter;
        state[13] = 0;
        state[14] = reinterpret_cast<const uint32_t*>(n)[0];
        state[15] = reinterpret_cast<const uint32_t*>(n)[1];
    }

    // Шифрует/дешифрует входной текст (операции симметричны)
    string process(const string& input) {
        string output = input;
        size_t len = input.size();
        uint8_t keystream[64];
        size_t offset = 0;

        while (len > 0) {
            generateBlock(keystream);
            size_t blockSize = min(len, size_t(64));
            for (size_t i = 0; i < blockSize; ++i) {
                output[offset + i] ^= keystream[i];
            }
            ++state[12]; // увеличиваем счётчик блока
            offset += blockSize;
            len -= blockSize;
        }
        return output;
    }

private:
    uint32_t state[16];

    // Основной блок преобразования: перестановка состояния с помощью раундов
    void generateBlock(uint8_t output[64]) {
        uint32_t workingState[16];
        memcpy(workingState, state, sizeof(state));
        for (int i = 0; i < 10; ++i) {
            // Вертикальные раунды
            quarterRound(workingState[0], workingState[4], workingState[8], workingState[12]);
            quarterRound(workingState[1], workingState[5], workingState[9], workingState[13]);
            quarterRound(workingState[2], workingState[6], workingState[10], workingState[14]);
            quarterRound(workingState[3], workingState[7], workingState[11], workingState[15]);
            // Диагональные раунды
            quarterRound(workingState[0], workingState[5], workingState[10], workingState[15]);
            quarterRound(workingState[1], workingState[6], workingState[11], workingState[12]);
            quarterRound(workingState[2], workingState[7], workingState[8], workingState[13]);
            quarterRound(workingState[3], workingState[4], workingState[9], workingState[14]);
        }
        for (int i = 0; i < 16; ++i) {
            workingState[i] += state[i];
        }
        memcpy(output, workingState, 64);
    }

    // Операция quarter round — базовая операция смешивания данных
    static void quarterRound(uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d) {
        a += b; d ^= a; d = rotl(d, 16);
        c += d; b ^= c; b = rotl(b, 12);
        a += b; d ^= a; d = rotl(d, 8);
        c += d; b ^= c; b = rotl(b, 7);
    }

    // Побитовый сдвиг влево
    static uint32_t rotl(uint32_t value, int count) {
        return (value << count) | (value >> (32 - count));
    }
};

string ChaCha20Algorithm::encrypt(const string& text, const string& key) const {
    auto pos = key.find(':');
    if (pos == string::npos) {
        throw invalid_argument("Ключ должен содержать ':' между ключом и sigma");
    }
    string keyPart = key.substr(0, pos);
    string sigmaPart = key.substr(pos + 1);

    ChaCha20 chacha(keyPart, sigmaPart);
    string raw = chacha.process(text);
    return base64_encode(raw);
}

string ChaCha20Algorithm::decrypt(const string& text, const string& key) const {
    auto pos = key.find(':');
    if (pos == string::npos) {
        throw invalid_argument("Ключ должен содержать ':' между ключом и sigma");
    }
    string keyPart = key.substr(0, pos);
    string sigmaPart = key.substr(pos + 1);

    ChaCha20 chacha(keyPart, sigmaPart);
    string raw = base64_decode(text);
    return chacha.process(raw);
}

string ChaCha20Algorithm::getName() const {
    return "chacha20";
}

string ChaCha20Algorithm::getDisplayName() const {
    return "Шифровка ChaCha20";
}

string ChaCha20Algorithm::getInputLabel() const {
    return "Ключ (32 байта):";
}

string ChaCha20Algorithm::getInputPlaceholder() const {
    return "например: 0123456789abcdef0123456789abcdef:expand 32-byte k";
}

ValidationResult ChaCha20Algorithm::validateInput(const string& input) const {
    auto pos = input.find(':');
    if (pos == string::npos) {
        return ValidationResult(false, "Ключ должен содержать ':' между ключом и sigma");
    }
    string keyPart = input.substr(0, pos);
    string sigmaPart = input.substr(pos + 1);

    if (keyPart.size() != 32) {
        return ValidationResult(false, "Часть ключа должна быть длиной ровно 32 символа");
    }
    if (sigmaPart.size() != 16) {
        return ValidationResult(false, "Sigma должна быть длиной ровно 16 символов");
    }
    return ValidationResult(true);
}