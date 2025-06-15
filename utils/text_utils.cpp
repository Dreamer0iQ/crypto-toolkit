#include "../utils/base64.h"
#include "../core/key_generator.h"
#include "../utils/dylib_utils.h"
#include <fstream>
#include <thread>
#include <atomic>
#include <cstring>
#include <dlfcn.h>
#include <filesystem>
#include <codecvt>
#include <locale>

bool isValidUnixPath(const string &path) {
    string filename = filesystem::path(path).filename().string();
    return !filename.empty() && filename.find('/') == string::npos &&
           filename.find('\0') == string::npos;
}

wstring stringToWstring(const string &str) {
    wstring_convert<codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes(str);
}

string wstringToString(const wstring &wstr) {
    wstring_convert<codecvt_utf8<wchar_t>> converter;
    return converter.to_bytes(wstr);
}

// Чтение файла с поддержкой base64 кодирования
string readFileToString(const string& filePath, bool base64) {
    ifstream file(filePath, ios::binary | ios::ate);
    if (!file) {
        throw runtime_error("Cannot open file: " + filePath);
    }

    size_t fileSize = file.tellg();
    file.seekg(0, ios::beg);

    string content;
    content.resize(fileSize);
    
    if (!file.read(&content[0], fileSize)) {
        throw runtime_error("Error reading file: " + filePath);
    }

    // Всегда кодируем в Base64 для бинарных файлов
    return base64 ? base64_encode(content) : content;
}

// Запись строки в файл с поддержкой base64 декодирования
void writeStringToFile(const string& filePath, 
                      const string& content, 
                      bool base64) {
    ofstream file(filePath, ios::binary);
    if (!file) {
        throw runtime_error("Cannot create file: " + filePath);
    }

    const string& data = base64 ? base64_decode(content) : content;
    
    if (!file.write(data.data(), data.size())) {
        throw runtime_error("Write error: " + filePath);
    }

    // Явная проверка ошибок записи
    if (!file.good()) {
        throw runtime_error("Write operation failed: " + filePath);
    }
}

// Определение, является ли файл текстовым
bool isTextFile(const string& filePath) {
    // Проверка по расширению
    string ext = filesystem::path(filePath).extension().string();
    transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    vector<string> textExtensions = {
        ".txt", ".md", ".cpp", ".c", ".h", ".hpp", ".py", ".js", ".html", 
        ".css", ".xml", ".json", ".csv", ".log", ".ini", ".cfg", ".conf"
    };
    
    for (const auto& textExt : textExtensions) {
        if (ext == textExt) return true;
    }
    
    // Дополнительная проверка содержимого для файлов без расширения
    ifstream file(filePath, ios::binary);
    if (!file.is_open()) return false;
    
    char buffer[512];
    file.read(buffer, sizeof(buffer));
    streamsize bytesRead = file.gcount();
    
    // Проверяем на наличие нулевых байтов (признак бинарного файла)
    for (streamsize i = 0; i < bytesRead; ++i) {
        if (buffer[i] == 0) return false;
    }
    
    return true;
}