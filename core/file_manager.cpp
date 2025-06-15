#include "file_manager.h"
#include "../utils/text_utils.h"
#include <fstream>
#include <filesystem>

using namespace std;

bool loadTextFromFile(const string& path, string& outText) {
    try {
        outText = readFileToString(path, false);
        return true;
    } catch (...) {
        return false;
    }
}

bool saveTextToFile(const string& path, const string& text) {
    ofstream outFile(path);
    if (!outFile) return false;
    outFile << text;
    return true;
}

bool loadKeyFromFile(const string& path, string& outKey) {
    try {
        outKey = readFileToString(path, false);
        
        // Удаляем возможные символы новой строки
        if (!outKey.empty() && outKey.back() == '\n') {
            outKey.pop_back();
        }
        return true;
    } catch (...) {
        return false;
    }
}

bool saveKeyToFile(const string& path, const string& key) {
    try {
        writeStringToFile(path, key, false);
        return true;
    } catch (...) {
        return false;
    }
}

bool fileExists(const string& path) {
    return filesystem::exists(path);
}

void ensureDirectoryExists(const string& dir) {
    namespace fs = filesystem;
    fs::path dir_path(dir);
    
    if (!fs::exists(dir_path)) {
        fs::create_directories(dir_path);
    }
}