#include <cmath>

extern "C" float E(int x) {
    // сумма ряда e = sum_{n=0..x} 1/n! (реализация 2)
    float sum = 0.0f;
    float fact = 1.0f;
    for (int n = 0; n <= x; ++n) {
        if (n > 0) fact *= n;
        sum += 1.0f / fact;
    }
    return sum;
}
