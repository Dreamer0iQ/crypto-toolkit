#include "console_utils.h"
#include <iostream>
#include <limits>

using namespace std;

#ifdef _WIN32
#include <windows.h>
#endif

void enableVirtualTerminal() {
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= 0x0004;
    SetConsoleMode(hOut, dwMode);
#endif
}

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pause() {
    cout << "\033[33mНажмите Enter, чтобы продолжить...\033[0m";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}