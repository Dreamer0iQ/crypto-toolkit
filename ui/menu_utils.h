#pragma once
#include "menu.h"

class Menu_Utils {
public:
    static void printBanner();
    static void showMainMenu();
    static void clearScreen();
    static void pause();
    static bool loadTextFromFile(const std::string& path, std::string& out);
    static bool isLibraryAvailable(const std::string& libPath);
};