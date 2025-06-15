#pragma once
#include <string>

using namespace std;

bool loadTextFromFile(const string& path, string& outText);
bool saveTextToFile(const string& path, const string& text);

bool loadKeyFromFile(const string& path, string& outKey);
bool saveKeyToFile(const string& path, const string& key);

bool fileExists(const string& path);
void ensureDirectoryExists(const string& dir);