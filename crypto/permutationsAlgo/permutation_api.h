#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void encrypt_permutations(const char* input, const char* key, char* output, int outputSize);
void decrypt_permutations(const char* input, const char* key, char* output, int outputSize);

#ifdef __cplusplus
}
#endif