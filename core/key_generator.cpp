#include "key_generator.h"
#include <algorithm>
#include <random>
#include <string>
#include <locale>
#include <codecvt>

using namespace std;

// Символы для сложных ключей: буквы, цифры, спецсимволы
const string complexCharset = 
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789"
    "!@#$%^&*()-_=+[]{};,.<>/?";

string generateMickey2Key() {
    string keyCharset = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    shuffle(keyCharset.begin(), keyCharset.end(), default_random_engine(random_device{}()));
    string key = keyCharset.substr(0, 10);

    shuffle(keyCharset.begin(), keyCharset.end(), default_random_engine(random_device{}()));
    string iv = keyCharset.substr(0, 10);

    return key + ":" + iv;
}

string generatePermutationKey() {
    // Количество перестановок от 100 до 10000
    int n = rand() % 9901 + 100; // [100, 10000]
    return to_string(n);
}

string generatePermutationsKey() {
    wstring russianCharset = 
        L"АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ"
        L"абвгдеёжзийклмнопрстуфхцчшщъыьэюя";
    shuffle(russianCharset.begin(), russianCharset.end(), default_random_engine(random_device{}()));

    int keyLength = rand() % 9001 + 1000;

    wstring wkey = russianCharset.substr(0, keyLength);

    // Преобразуем wstring (UTF-16/32) в string (UTF-8)
    wstring_convert<codecvt_utf8<wchar_t>> converter;
    return converter.to_bytes(wkey);
}

string generateChaCha20Key() {
    string key = complexCharset;
    shuffle(key.begin(), key.end(), default_random_engine(random_device{}()));
    return key.substr(0, 32); // 32 байта = 256 бит
}

string generateChaCha20Sigma() {
    string sigma = complexCharset;
    shuffle(sigma.begin(), sigma.end(), default_random_engine(random_device{}()));
    return sigma.substr(0, 16); // 16 байт
}