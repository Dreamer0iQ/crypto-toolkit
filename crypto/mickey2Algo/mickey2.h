#pragma once
#include "../crypto_algorithm.h"
#include <string>
#include <vector>

using namespace std;

class Mickey2Algorithm : public CryptoAlgorithm {
public:
    string getName() const override;
    string getDisplayName() const override;
    string getInputLabel() const override;
    string getInputPlaceholder() const override;

    ValidationResult validateInput(const string& input) const override;
    string encrypt(const string& text, const string& key) const override;
    string decrypt(const string& text, const string& key) const override;
};