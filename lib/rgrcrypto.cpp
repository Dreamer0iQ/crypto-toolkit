#include "rgrcrypto.h"
#include "../crypto/mickey2Algo/mickey2.h"
#include <string>

using namespace std;

static Mickey2Algorithm cipher;
static string result;  // Статическая переменная для хранения результата