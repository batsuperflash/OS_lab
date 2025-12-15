#include <iostream>
#include <dlfcn.h>
#include <string>
#include <sys/stat.h>
#include "contracts.h"  

// Проверка существования файла
static bool exists_file(const std::string &path) {
    struct stat sb;
    return (stat(path.c_str(), &sb) == 0 && S_ISREG(sb.st_mode));
}

int main() {
    const std::string lib1 = "libs/libcalc1.so";
    const std::string lib2 = "libs/libcalc2.so";

    std::string curLib = lib1;
    if (!exists_file(curLib)) {
        std::cerr << "Library not found: " << curLib << "\n";
        return 1;
    } //  в strace: newfstatat

    // загружаю библиотеку в память
    // в strace: openat(libcalc1.so),  read заголовка,  mmap 
    void* handle = dlopen(curLib.c_str(), RTLD_LAZY);
    if (!handle) {
        std::cerr << "dlopen failed: " << dlerror() << std::endl;
        return 1;
    }
    
    // получаю указатель на функцию derivative по имени
    // Загружаем символы, используя объявления из contracts.h
    float (*Derivative_ptr)(float, float) = (float(*)(float, float))dlsym(handle, "Derivative");
    const char* err1 = dlerror();
    if (err1) {
        std::cerr << "Symbol 'Derivative' not found: " << err1 << "\n";
        dlclose(handle);
        return 1;
    }
    
    // то же самое для e
    float (*E_ptr)(int) = (float(*)(int))dlsym(handle, "E");
    const char* err2 = dlerror();
    if (err2) {
        std::cerr << "Symbol 'E' not found: " << err2 << "\n";
        dlclose(handle);
        return 1;
    }

    int cmd;
    while (std::cin >> cmd) {
        // в strace: read(0, "1 12 3\n", ...)
        if (cmd == 0) {
            // // переключаюсь на вторую библиотеку
            dlclose(handle); //  munmap: выгрузка первой библиотеки
            std::string other = lib2;
            if (!exists_file(other)) {
                std::cerr << "Other library not found: " << other << "\n";
                return 1;
            }
            // загружаю новую — в strace снова openat + mmap, но для второй либы
            handle = dlopen(other.c_str(), RTLD_LAZY);
            if (!handle) {
                std::cerr << "dlopen switch failed: " << dlerror() << "\n";
                return 1;
            }

            Derivative_ptr = (float(*)(float, float))dlsym(handle, "Derivative");
            err1 = dlerror();
            if (err1) {
                std::cerr << "Symbol 'Derivative' not found after switch: " << err1 << "\n";
                dlclose(handle);
                return 1;
            }

            E_ptr = (float(*)(int))dlsym(handle, "E");
            err2 = dlerror();
            if (err2) {
                std::cerr << "Symbol 'E' not found after switch: " << err2 << "\n";
                dlclose(handle);
                return 1;
            }

            std::cout << "Switched library to: " << other << "\n";
        }
        else if (cmd == 1) {
            float A, d;
            // вызываю функцию и вывожу результат
            // в strace: write
            if (!(std::cin >> A >> d)) break;
            std::cout << Derivative_ptr(A, d) << std::endl;
        }
        else if (cmd == 2) {
            int x;
            if (!(std::cin >> x)) break;
            // вызов e(x) и вывод
            std::cout << E_ptr(x) << std::endl;
        }
    }
    // завершение: выгружаю библиотеку - munmap в strace
    dlclose(handle);
    return 0;
}