#include <filesystem>
#pragma once
#include <string>

enum class Mode {
    ENCRYPT,
    DECRYPT
};

using namespace std;

struct ValidationResult {
    bool isValid;
    string error;

    ValidationResult(bool valid = true, const string& err = "")
        : isValid(valid), error(err) {}
};

class CryptoAlgorithm {
public:
    virtual ~CryptoAlgorithm() = default;

    virtual string getName() const = 0;
    virtual string getDisplayName() const = 0;
    virtual string getInputLabel() const = 0;
    virtual string getInputPlaceholder() const = 0;

    virtual ValidationResult validateInput(const string& input) const = 0;
    virtual string encrypt(const string& text, const string& key) const = 0;
    virtual string decrypt(const string& text, const string& key) const = 0;
};