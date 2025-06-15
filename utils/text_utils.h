#ifndef TEXT_UTILS_H
#define TEXT_UTILS_H

#include <string>

bool fileExists(const std::string &path);
bool isValidUnixPath(const std::string &path);

std::wstring stringToWstring(const std::string &str);
std::string wstringToString(const std::wstring &wstr);

std::string readFileToString(const std::string &filePath, bool base64 = false);
void writeStringToFile(const std::string &filePath, const std::string &content, bool base64 = false);

bool isTextFile(const std::string &filePath);

#endif // TEXT_UTILS_H