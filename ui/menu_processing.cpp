#include "menu_processing.h"
#include "menu_utils.h"
#include "../utils/base64.h"
#include "../utils/text_utils.h"

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

using namespace std;

struct DynamicCrypto {
    using FuncPtr = int(*)(const char*, const char*, char*, int);

    void* handle = nullptr;
    FuncPtr encrypt = nullptr;
    FuncPtr decrypt = nullptr;

    bool load(const string& path, const string& encryptName, const string& decryptName) {
        handle = dlopen(path.c_str(), RTLD_LAZY);
        if (!handle) {
            cerr << "dlopen error: " << dlerror() << endl;
            return false;
        }

        encrypt = (FuncPtr)dlsym(handle, encryptName.c_str());
        decrypt = (FuncPtr)dlsym(handle, decryptName.c_str());

        if (!encrypt || !decrypt) {
            cerr << "dlsym error: " << dlerror() << endl;
            dlclose(handle);
            return false;
        }

        return true;
    }

    void unload() {
        if (handle) dlclose(handle);
        handle = nullptr;
    }
};

bool loadUniversalFile(const string& filePath, string& data, bool& wasBase64Encoded) {
    if (!fileExists(filePath)) {
        return false;
    }
    
    try {
        bool isText = isTextFile(filePath);
        wasBase64Encoded = !isText;
        
        if (isText) {
            cout << "\033[36mОбнаружен текстовый файл, обработка без base64...\033[0m\n";
            data = readFileToString(filePath, false);
        } else {
            cout << "\033[36mОбнаружен бинарный файл, применяется base64 кодирование...\033[0m\n";
            data = readFileToString(filePath, true);
        }
        return true;
    } catch (const exception& e) {
        cerr << "\033[31mОшибка при загрузке файла: " << e.what() << "\033[0m\n";
        return false;
    }
}

string generateOutputPath(const string& inputPath, const string& operation, const string& algorithm) {
    filesystem::path path(inputPath);
    string baseName = path.stem().string();
    string extension = path.extension().string();
    string parentPath = path.parent_path().string();
    
    // Если родительский путь пустой, используем текущую директорию
    if (parentPath.empty()) parentPath = ".";
    
    string newFileName = baseName + "_" + algorithm + "_" + operation + extension;
    
    // Формируем полный путь
    filesystem::path fullPath = filesystem::path(parentPath) / newFileName;
    return fullPath.string();
}

void processWithCrypto(const string& algorithm, const string& libPath, 
                      const string& encryptFunc, const string& decryptFunc, 
                      const string& key) {
    string filePath;
    cout << "Введите путь к файлу: ";
    getline(cin, filePath);
    
    if (!fileExists(filePath)) {
        cerr << "\033[31mФайл не найден: " << filePath << "\033[0m\n";
        cout << "Нажмите Enter для продолжения...";
        cin.get();
        return;
    }
    
    string fileData;
    bool wasBase64Encoded = false;
    
    if (!loadUniversalFile(filePath, fileData, wasBase64Encoded)) {
        cerr << "\033[31mНе удалось загрузить файл: " << filePath << "\033[0m\n";
        cout << "Нажмите Enter для продолжения...";
        cin.get();
        return;
    }

    DynamicCrypto crypto;
    if (!crypto.load(libPath, encryptFunc, decryptFunc)) {
        cout << "\033[31mНе удалось загрузить " << algorithm << " библиотеку.\033[0m\n";
        cout << "Нажмите Enter для продолжения...";
        cin.get();
        return;
    }

    size_t bufferSize = fileData.size() * 2 + 1024; // Увеличиваем буфер для надежности
    char* output = new char[bufferSize];

    int mode = 0;
    cout << "\033[36mВыберите режим:\033[0m\n";
    cout << "\033[32m1. Шифрование\033[0m\n";
    cout << "\033[32m2. Дешифрование\033[0m\n";
    cout << "\033[36mВведите номер: \033[0m";
    cin >> mode;
    cin.ignore();

    if (mode != 1 && mode != 2) {
        cout << "\033[31mНекорректный режим.\033[0m\n";
        delete[] output;
        crypto.unload();
        cout << "Нажмите Enter для продолжения...";
        cin.get();
        return;
    }

    atomic<bool> done(false);
    thread loader([&done]() {
        const char symbols[] = {'|', '/', '-', '\\'};
        int idx = 0;
        while (!done) {
            cout << "\r\033[36mОбработка... " << symbols[idx++ % 4] << "\033[0m" << flush;
            this_thread::sleep_for(chrono::milliseconds(150));
        }
        cout << "\r\033[36mОбработка завершена.        \033[0m\n";
    });
    
    try {
        int outLen = 0;
        if (mode == 1) {
            outLen = crypto.encrypt(fileData.c_str(), key.c_str(), output, static_cast<int>(bufferSize));
        } else {
            outLen = crypto.decrypt(fileData.c_str(), key.c_str(), output, static_cast<int>(bufferSize));
        }
        
        if (outLen < 0) {
            throw runtime_error("Crypto operation failed");
        }
        
        done = true;
        loader.join();

        string result(output, outLen);
        
        // Показываем результат только для текстовых файлов при дешифровании
        if (mode == 2 && !wasBase64Encoded) {
            cout << "\033[36mРезультат дешифровки:\033[0m\n";
            if (result.size() <= 1024) {
                cout << result << '\n';
            } else {
                cout << "\033[36mПервые 1024 символа:\033[0m\n";
                cout << result.substr(0, 1024) << "\n...\n";
            }
        } else if (mode == 2) {
            cout << "\033[36mБинарный файл успешно дешифрован.\033[0m\n";
        }

        string outPath;
        cout << "Введите путь для сохранения результата (или Enter для автоматического): ";
        getline(cin, outPath);

        string operation = (mode == 1) ? "encrypted" : "decrypted";
        string autoPath = generateOutputPath(filePath, operation, algorithm);

        if (outPath.empty() || outPath == "./" || outPath == "." || outPath.back() == '/') {
            filesystem::path dir = (outPath.back() == '/') ? filesystem::path(outPath) : filesystem::path(".");
            outPath = (dir / filesystem::path(autoPath).filename()).string();
        }

        try {
            // Проверяем, что директория существует
            filesystem::path outputPath(outPath);
            filesystem::path outputDir = outputPath.parent_path();
            
            if (!outputDir.empty() && !filesystem::exists(outputDir)) {
                filesystem::create_directories(outputDir);
                cout << "\033[36mСоздана директория: " << outputDir << "\033[0m\n";
            }
            
            // При дешифровании бинарных файлов нужно декодировать base64
            bool shouldDecodeBase64 = wasBase64Encoded;
            writeStringToFile(outPath, result, shouldDecodeBase64);
            
            cout << "\033[34mУспешно сохранено " << result.size() << " байт в " << outPath << "\033[0m\n";
            
            if (mode == 2 && wasBase64Encoded) {
                cout << "\033[36mБинарный файл восстановлен из base64.\033[0m\n";
            }
        } catch (const exception& e) {
            cout << "\033[31mОшибка при сохранении: " << e.what() << "\033[0m\n";
            cout << "\033[33mПопытка сохранения в текущую директорию...\033[0m\n";
            
            // Fallback - сохраняем в текущую директорию с автоматическим именем
            try {
                string operation = (mode == 1) ? "encrypted" : "decrypted";
                string fallbackPath = filesystem::path(filePath).stem().string() + 
                                    "_" + algorithm + "_" + operation + 
                                    filesystem::path(filePath).extension().string();
                
                bool shouldDecodeBase64 = wasBase64Encoded;
                writeStringToFile(fallbackPath, result, shouldDecodeBase64);
                cout << "\033[34mУспешно сохранено в: " << fallbackPath << "\033[0m\n";
            } catch (const exception& fallbackError) {
                cout << "\033[31mНе удалось сохранить файл: " << fallbackError.what() << "\033[0m\n";
            }
        }
        
    } catch (const exception& e) {
        done = true;
        loader.join();
        cout << "\033[31mОшибка при выполнении операции: " << e.what() << "\033[0m\n";
        delete[] output;
        crypto.unload();
        cout << "Нажмите Enter для продолжения...";
        cin.get();
        return;
    }

    delete[] output;
    crypto.unload();
    cout << "Нажмите Enter для продолжения...";
    cin.get();
}

void Menu::processPermutationWithKey(const string& key) {
    processWithCrypto("permutation", "bin/libpermutations.dylib", 
                     "encrypt_permutation", "decrypt_permutation", key);
}

void Menu::processMickey2WithKey(const string& key) {
    processWithCrypto("mickey2", "bin/libmickey2.dylib", 
                     "encrypt_mickey2", "decrypt_mickey2", key);
}

void Menu::processChacha20WithKey(const string& key) {
    processWithCrypto("chacha20", "bin/libchacha20.dylib", 
                     "encrypt_chacha20", "decrypt_chacha20", key);
}