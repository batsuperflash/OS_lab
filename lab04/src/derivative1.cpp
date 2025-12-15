#include <cmath>

extern "C" float Derivative(float A, float deltaX) {
    // односторонняя разность (реализация 1)
    return (cosf(A + deltaX) - cosf(A)) / deltaX;
}
