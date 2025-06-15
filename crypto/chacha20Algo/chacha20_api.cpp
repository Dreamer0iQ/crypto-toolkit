#include "chacha20_api.h"
#include "./chacha20.h"
#include <cstring>

static ChaCha20Algorithm cipher;

extern "C" {

int encrypt_chacha20(const char* input, const char* key, char* output, int bufferSize) {
    if (!input || !key || !output || bufferSize <= 1) {
        // Неверные параметры, не можем записать данные
        return 0;
    }

    std::string result = cipher.encrypt(input, key);
    int length = (int)result.size();

    if (length > bufferSize - 1) {
        length = bufferSize - 1;  // Ограничиваем размер по буферу
    }

    strncpy(output, result.c_str(), length);
    output[length] = '\0';  // Гарантируем нуль-терминатор

    return length;  // Возвращаем количество реально записанных байт (без нуль-терминатора)
}

int decrypt_chacha20(const char* input, const char* key, char* output, int bufferSize) {
    if (!input || !key || !output || bufferSize <= 1) {
        return 0;
    }

    std::string result = cipher.decrypt(input, key);
    int length = (int)result.size();

    if (length > bufferSize - 1) {
        length = bufferSize - 1;
    }

    strncpy(output, result.c_str(), length);
    output[length] = '\0';

    return length;
}

}