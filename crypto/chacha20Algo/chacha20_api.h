#pragma once

#ifdef __cplusplus
extern "C" {
#endif

int encrypt_chacha20(const char* input, const char* key, char* output, int outputSize);
int decrypt_chacha20(const char* input, const char* key, char* output, int outputSize);

#ifdef __cplusplus
}
#endif