#include <cmath>

extern "C" float E(int x) {
    // (1 + 1/x)^x - приближение (реализация 1)
    if (x <= 0) return 1.0f;
    return powf(1.0f + 1.0f / x, (float)x);
}
