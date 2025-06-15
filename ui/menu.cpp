#include "menu.h"
#include "../core/key_generator.h"
#include "menu_processing.h"
#include "menu_utils.h"

#include <thread>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <fstream>

using namespace std;

void Menu::run() {
    srand(time(nullptr));
    while (true) {
        clearScreen();
        printBanner();
        showMainMenu();

        int choice = 0;
        cout << "\033[36mВведите номер: \033[0m";
        cin >> choice;
        cin.ignore();

        if (choice == 0) {
            cout << "\033[33mВыход...\033[0m\n";
            break;
        }
        handleKeyInput(choice);
    }
}

void Menu::handleKeyInput(int choice) {
    string algoName;
    switch (choice) {
        case 1: algoName = "mickey2"; break;
        case 2: algoName = "chacha20"; break;
        case 3: algoName = "permutations"; break;
        default:
            cout << "\033[33mНеизвестный алгоритм.\033[0m\n";
            pause();
            return;
    }

    clearScreen();
    string prevKeyPath = "generated_keys/" + algoName + "_key.txt";
    ifstream prevKeyFile(prevKeyPath);
    string key;
    if (prevKeyFile) {
        cout << "\033[36mНайден ранее сгенерированный ключ. Использовать его? (y/n): \033[0m";
        string answer;
        getline(cin, answer);
        if (answer == "y" || answer == "Y") {
            getline(prevKeyFile, key);
            if (!key.empty()) {
                cout << "\033[34mКлюч загружен из " << prevKeyPath << "\033[0m\n";
                if (choice == 1) {
                    processMickey2WithKey(key);
                } else if (choice == 2) {
                    processChacha20WithKey(key);
                } else if (choice == 3) {
                    processPermutationWithKey(key);
                }
                return;
            }
        }
    }
    
    cout << "\033[36m1. Сгенерировать ключ\033[0m\n";
    cout << "\033[36m2. Ввести вручную\033[0m\n";
    cout << "\033[36m3. Назад в меню\033[0m\n";
    cout << "\033[36mВыберите способ ввода ключа: \033[0m";

    int option = 0;
    cin >> option;
    cin.ignore();

    if (option == 1) {
        if (algoName == "mickey2") {
            key = generateMickey2Key();
        } else if (algoName == "chacha20") {
            key = generateChaCha20Key() + ":" + generateChaCha20Sigma();
        } else if (algoName == "permutations") {
            key = generatePermutationsKey();
        }

        cout << "\033[32mСгенерированный ключ:\033[0m ";
        for (char c : key) {
            cout << c << flush;
            this_thread::sleep_for(chrono::milliseconds(40));
        }
        cout << '\n';

        system("mkdir -p generated_keys");
        string filename = "generated_keys/" + algoName + "_key.txt";
        ofstream keyFile(filename);
        if (keyFile) {
            keyFile << key;
            cout << "\033[34mКлюч сохранён в " << filename << "\033[0m\n";
        } else {
            cout << "\033[31mНе удалось сохранить ключ.\033[0m\n";
        }
    } else if (option == 2) {
        cout << "Введите ключ: ";
        getline(cin, key);
    } else {
        return;
    }

    if (choice == 1) {
        processMickey2WithKey(key);
    } else if (choice == 2) {
        processChacha20WithKey(key);
    } else if (choice == 3) {
        processPermutationWithKey(key);
    }
}