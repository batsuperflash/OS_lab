#include <iostream>
#include "contracts.h"

// исп библиотеку на этапе линковки


int main() {
    int cmd;
    while (std::cin >> cmd) {
        if (cmd == 1) {
            float A, d;
            if (!(std::cin >> A >> d)) break;
            std::cout << Derivative(A, d) << std::endl;
        } else if (cmd == 2) {
            int x;
            if (!(std::cin >> x)) break;
            std::cout << E(x) << std::endl;
        } else {
            // Игнорируем прочие команды
        }
    }
    return 0;
}






