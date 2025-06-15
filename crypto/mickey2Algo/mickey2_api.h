#pragma once

#ifdef __cplusplus
extern "C" {
#endif

int encrypt_mickey2(const char* input, const char* key, char* output, int outputSize);
int decrypt_mickey2(const char* input, const char* key, char* output, int outputSize);

#ifdef __cplusplus
}
#endif