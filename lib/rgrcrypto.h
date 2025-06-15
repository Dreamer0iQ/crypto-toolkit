#ifndef RGRCRYPTO_H
#define RGRCRYPTO_H

#ifdef __cplusplus
extern "C" {
#endif

const char* encrypt_mickey2(const char* input, const char* key);
const char* decrypt_mickey2(const char* input, const char* key);

// другие функции по мере необходимости

#ifdef __cplusplus
}
#endif

#endif // RGRCRYPTO_H