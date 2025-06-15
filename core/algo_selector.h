#pragma once
#include <memory>
#include <string>
#include "../crypto/crypto_algorithm.h"

std::unique_ptr<CryptoAlgorithm> createAlgorithm(int choice);
std::string getAlgoName(int choice);