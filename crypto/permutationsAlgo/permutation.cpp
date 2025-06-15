#include "../../utils/base64.h"
#include "./permutation.h"
#include <vector>
#include <string>
#include <stdexcept>
#include <sstream>
#include <algorithm>
#include <random>
using namespace std;

class PermutationCipher {
private:
    vector<int> perm;
    vector<int> invPerm;
    int k;

    // Генерируем перестановку на основе числового ключа
    vector<int> generatePermutation(int key, int blockSize) {
        vector<int> result(blockSize);
        
        // Инициализируем массив [0, 1, 2, ..., blockSize-1]
        for (int i = 0; i < blockSize; ++i) {
            result[i] = i;
        }
        
        // Используем ключ как seed для генератора
        mt19937 rng(key);
        
        // Перемешиваем массив
        shuffle(result.begin(), result.end(), rng);
        
        return result;
    }

public:
    PermutationCipher(int key, int blockSize = 8) {
        k = blockSize;
        perm = generatePermutation(key, k);
        
        // Создаем обратную перестановку
        invPerm.resize(k);
        for (int i = 0; i < k; ++i) {
            invPerm[perm[i]] = i;
        }
    }

    string encrypt(const string& input) const {
        string result;
        size_t len = input.size();
        
        // Обрабатываем по блокам размера k
        for (size_t i = 0; i < len; i += k) {
            string block = input.substr(i, k);
            
            // Дополняем блок до размера k, если нужно
            while (block.size() < k) {
                block += ' '; // Используем пробелы для дополнения
            }
            
            // Применяем перестановку: символ с позиции j идет на позицию perm[j]
            string encryptedBlock(k, ' ');
            for (int j = 0; j < k; ++j) {
                encryptedBlock[perm[j]] = block[j];
            }
            
            result += encryptedBlock;
        }
        
        return result;
    }
    
    string decrypt(const string& input) const {
        string result;
        size_t len = input.size();
        
        // Обрабатываем по блокам размера k
        for (size_t i = 0; i < len; i += k) {
            string block = input.substr(i, k);
            
            // Дополняем блок до размера k, если нужно
            while (block.size() < k) {
                block += ' ';
            }
            
            // Применяем обратную перестановку: символ с позиции j идет на позицию invPerm[j]
            string decryptedBlock(k, ' ');
            for (int j = 0; j < k; ++j) {
                decryptedBlock[invPerm[j]] = block[j];
            }
            
            result += decryptedBlock;
        }
        
        return result;
    }
};

string PermutationAlgorithm::encrypt(const string& text, const string& key) const {
    // Преобразуем строковый ключ в число
    int numericKey;
    try {
        numericKey = stoi(key);
    } catch (const exception&) {
        throw invalid_argument("Ключ должен быть числом");
    }
    
    PermutationCipher cipher(numericKey);
    string raw = cipher.encrypt(text);
    return base64_encode(raw);
}

string PermutationAlgorithm::decrypt(const string& text, const string& key) const {
    // Преобразуем строковый ключ в число
    int numericKey;
    try {
        numericKey = stoi(key);
    } catch (const exception&) {
        throw invalid_argument("Ключ должен быть числом");
    }
    
    PermutationCipher cipher(numericKey);
    string raw = base64_decode(text);
    string decrypted = cipher.decrypt(raw);
    
    // Удаляем trailing пробелы (дополнение)
    while (!decrypted.empty() && decrypted.back() == ' ') {
        decrypted.pop_back();
    }
    
    return decrypted;
}

string PermutationAlgorithm::getName() const {
    return "permutation";
}

string PermutationAlgorithm::getDisplayName() const {
    return "Шифровка зафиксированной перестановкой";
}

string PermutationAlgorithm::getInputLabel() const {
    return "Числовой ключ (например, 12345):";
}

string PermutationAlgorithm::getInputPlaceholder() const {
    return "например: 12345";
}

ValidationResult PermutationAlgorithm::validateInput(const string& input) const {
    if (input.empty()) {
        return ValidationResult(false, "Ключ не может быть пустым");
    }
    
    try {
        int key = stoi(input);
        if (key < 0) {
            return ValidationResult(false, "Ключ должен быть положительным числом");
        }
    } catch (const exception&) {
        return ValidationResult(false, "Ключ должен быть числом");
    }
    
    return ValidationResult(true);
}