#pragma once
#include <dlfcn.h>
#include <string>
#include <iostream>

inline bool isLibraryAvailable(const std::string& libPath) {
    void* handle = dlopen(libPath.c_str(), RTLD_LAZY);
    if (!handle) {
        std::cerr << "\033[31mОшибка загрузки библиотеки " << libPath << "\033[0m\n";
        return false;
    }
    dlclose(handle);
    return true;
}