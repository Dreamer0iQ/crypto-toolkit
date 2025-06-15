#include "permutation_api.h"
#include "./permutation.h"
#include <cstring>

static PermutationAlgorithm cipher;

extern "C" {

int encrypt_permutation(const char* input, const char* key, char* output, int bufferSize) {
    if (!input || !key || !output || bufferSize <= 1) {
        return 0;
    }

    std::string result = cipher.encrypt(input, key);
    int length = (int)result.size();

    if (length > bufferSize - 1) {
        length = bufferSize - 1;
    }

    strncpy(output, result.c_str(), length);
    output[length] = '\0';

    return length;
}

int decrypt_permutation(const char* input, const char* key, char* output, int bufferSize) {
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