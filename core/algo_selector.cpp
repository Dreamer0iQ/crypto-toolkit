#include "algo_selector.h"
#include "../utils/dylib_utils.h"
#include "../crypto/mickey2Algo/mickey2.h"
#include "../crypto/chacha20Algo/chacha20.h"
#include "../crypto/permutationsAlgo/permutation.h"
#include <dlfcn.h>
#include <iostream>

using namespace std;

unique_ptr<CryptoAlgorithm> createAlgorithm(int choice) {
    string libPath;
    switch(choice) {
        case 1: libPath = "bin/libmickey2.dylib"; break;
        case 2: libPath = "bin/libchacha20.dylib"; break;
        case 3: libPath = "bin/libpermutations.dylib"; break;
        default: libPath = ""; break;
    }

    if (!libPath.empty()) {
        if (!isLibraryAvailable(libPath)) {
            cerr << "\033[31mОшибка загрузки библиотеки " << libPath << "\033[0m\n";
            return nullptr;
        }
    }

    switch (choice) {
        case 1: return make_unique<Mickey2Algorithm>();
        case 2: return make_unique<ChaCha20Algorithm>();
        case 3: return make_unique<PermutationAlgorithm>();
        default: return nullptr;
    }
}

string getAlgoName(int choice) {
    switch (choice) {
        case 1: return "mickey2";
        case 2: return "chacha20";
        case 3: return "permutations";
        default: return "unknown";
    }
}