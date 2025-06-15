#pragma once
#include "menu.h"

class Menu_Processing {
public:
    static void processPermutationWithKey(const std::string& key);
    static void processChacha20WithKey(const std::string& key);
    static void processMickey2WithKey(const std::string& key);
};