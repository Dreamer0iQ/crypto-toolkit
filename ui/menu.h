#pragma once

#include <string>

class Menu {
public:
    void run();
    
private:
    void handleKeyInput(int choice);
    void processPermutationWithKey(const std::string& key);
    void processChacha20WithKey(const std::string& key);
    void processMickey2WithKey(const std::string& key);
    
    // Вспомогательные методы
    void showMainMenu();
    void printBanner();
    void clearScreen();
    void pause();
    bool loadTextFromFile(const std::string& path, std::string& out);
    bool isLibraryAvailable(const std::string& libPath);
};