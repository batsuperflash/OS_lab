#include <iostream>
#include <dlfcn.h>
#include <string>
#include <memory>
#include <sys/stat.h>
#include "contracts.h"

// Вспомогательная функция: существует ли файл?
static bool exists_file(const std::string &path) {
    struct stat sb;
    return (stat(path.c_str(), &sb) == 0 && S_ISREG(sb.st_mode));
}

int main(int argc, char** argv) {
    // По умолчанию ищем libs/libcalc1.dylib (mac) или libs/libcalc1.so (linux)
    std::string lib1_dylib = "libs/libcalc1.dylib";
    std::string lib2_dylib = "libs/libcalc2.dylib";
    std::string lib1_so = "libs/libcalc1.so";
    std::string lib2_so = "libs/libcalc2.so";

    std::string curLib;
    if (exists_file(lib1_dylib)) curLib = lib1_dylib;
    else if (exists_file(lib1_so)) curLib = lib1_so;
    else {
        std::cerr << "No library found (tried " << lib1_dylib << " and " << lib1_so << ")\n";
        return 1;
    }

    void* handle = dlopen(curLib.c_str(), RTLD_LAZY);
    if (!handle) {
        std::cerr << "dlopen failed: " << dlerror() << std::endl;
        return 1;
    }

    auto load_symbols = [&](void* h, float(**der)(float,float), float(**ee)(int)) -> bool {
        dlerror();
        *der = (float(*)(float,float))dlsym(h, "Derivative");
        const char* e1 = dlerror();
        if (e1) return false;
        *ee = (float(*)(int))dlsym(h, "E");
        const char* e2 = dlerror();
        if (e2) return false;
        return true;
    };

    float (*Derivative_ptr)(float,float) = nullptr;
    float (*E_ptr)(int) = nullptr;
    if (!load_symbols(handle, &Derivative_ptr, &E_ptr)) {
        std::cerr << "Failed to load symbols from " << curLib << "\n";
        dlclose(handle);
        return 1;
    }

    int cmd;
    while (std::cin >> cmd) {
        if (cmd == 0) {
            // переключиться на другую реализацию (lib2)
            dlclose(handle);
            std::string other = exists_file(lib2_dylib) ? lib2_dylib : lib2_so;
            if (!exists_file(other)) {
                std::cerr << "Other library not found: " << other << "\n";
                return 1;
            }
            handle = dlopen(other.c_str(), RTLD_LAZY);
            if (!handle) {
                std::cerr << "dlopen switch failed: " << dlerror() << "\n";
                return 1;
            }
            if (!load_symbols(handle, &Derivative_ptr, &E_ptr)) {
                std::cerr << "Failed to load symbols after switch\n";
                dlclose(handle);
                return 1;
            }
            std::cout << "Switched library to: " << other << "\n";
        }
        else if (cmd == 1) {
            float A, d;
            if (!(std::cin >> A >> d)) break;
            std::cout << Derivative_ptr(A, d) << std::endl;
        }
        else if (cmd == 2) {
            int x;
            if (!(std::cin >> x)) break;
            std::cout << E_ptr(x) << std::endl;
        }
    }

    dlclose(handle);
    return 0;
}
