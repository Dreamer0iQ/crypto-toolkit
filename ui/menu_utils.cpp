#include "menu_utils.h"
#include "../utils/console_utils.h"
#include "../core/file_manager.h"
#include <iostream>
#include <fstream>
#include <dlfcn.h>
#include <thread>
#include <chrono>

using namespace std;

void Menu::printBanner() {
    cout << R"(
                                  # #### ####
                                ### \/#|### |/####
                               ##\/#/ \||/##/_/##/_#
                             ###  \/###|/ \/ # ###
                           ##_\_#\_\## | #/###_/_####     _____              __
                          ## #### # \ #| /  #### ##/##   / ___/_____ _____  / /____ 
                           __#_--###  |{,###---###-~    / /__/ __/ // / _ \/ __/ _ \
                                     \ }{               \___/_/  \_, / .__/\__/\___/
                                      }}{                       /___/_/    				
                                      }}{
                                 ejm  {{}
                                , -=-~{ .-^- _
                                      }
                                       {
)" << '\n';
}

void Menu::showMainMenu() {
    bool mickey2Available = isLibraryAvailable("bin/libmickey2.dylib");
    bool chachaAvailable = isLibraryAvailable("bin/libchacha20.dylib");
    bool permutationsAvailable = isLibraryAvailable("bin/libpermutations.dylib");

    cout << "\033[36mВыберите алгоритм:\033[0m\n";

    cout << (mickey2Available ? "\033[32m" : "\033[31m");
    cout << "1. mickey2" << (mickey2Available ? "" : " (недоступен)") << "\033[0m\n";

    cout << (chachaAvailable ? "\033[32m" : "\033[31m");
    cout << "2. Chacha20" << (chachaAvailable ? "" : " (недоступен)") << "\033[0m\n";

    cout << (permutationsAvailable ? "\033[32m" : "\033[31m");
    cout << "3. Permutations Cipher" << (permutationsAvailable ? "" : " (недоступен)") << "\033[0m\n";

    cout << "\033[31m0. Выход\033[0m\n";
}

void Menu::clearScreen() {
    #if defined(_WIN32)
        system("cls");
    #else
        system("clear");
    #endif
}

void Menu::pause() {
    cout << "\033[36mНажмите Enter для продолжения...\033[0m";
    cin.ignore();
}

bool Menu::loadTextFromFile(const string& path, string& out) {
    ifstream file(path, ios::binary);
    if (!file) {
        return false;
    }
    file.seekg(0, ios::end);
    streamsize size = file.tellg();
    file.seekg(0, ios::beg);
    out.resize(static_cast<size_t>(size));
    file.read(&out[0], size);
    return true;
}

bool Menu::isLibraryAvailable(const string& libPath) {
    void* handle = dlopen(libPath.c_str(), RTLD_LAZY);
    if (handle) {
        dlclose(handle);
        return true;
    }
    return false;
}