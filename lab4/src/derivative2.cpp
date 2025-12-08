#include <cmath>

extern "C" float Derivative(float A, float deltaX) {
    // центральная разность (реализация 2)
    return (cosf(A + deltaX) - cosf(A - deltaX)) / (2.0f * deltaX);
}
