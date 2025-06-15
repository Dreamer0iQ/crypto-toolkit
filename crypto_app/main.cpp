#include "ui/menu.h"
#include "utils/console_utils.h"

int main() {
    enableVirtualTerminal();
    Menu menu;
    menu.run();
    return 0;
}