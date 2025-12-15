# CXX = g++
# CXXFLAGS = -std=c++17 -fPIC -Wall
# INCLUDE = -Iinclude
# LIBDIR = libs

# .PHONY: all macos linux clean docker_build docker_run

# # по умолчанию соберём для macOS (если ты на macOS)
# all: macos_all

# # ----------------------------
# # macOS targets (.dylib)
# # ----------------------------
# macos_all: $(LIBDIR)/libcalc1.dylib $(LIBDIR)/libcalc2.dylib program1_macos program2_macos

# $(LIBDIR)/libcalc1.dylib:
# 	mkdir -p $(LIBDIR)
# 	$(CXX) $(CXXFLAGS) src/derivative1.cpp src/e1.cpp -dynamiclib -o $(LIBDIR)/libcalc1.dylib

# $(LIBDIR)/libcalc2.dylib:
# 	mkdir -p $(LIBDIR)
# 	$(CXX) $(CXXFLAGS) src/derivative2.cpp src/e2.cpp -dynamiclib -o $(LIBDIR)/libcalc2.dylib

# program1_macos:
# 	$(CXX) src/main_linking.cpp -L$(LIBDIR) -lcalc1 $(INCLUDE) -o program1

# program2_macos:
# 	$(CXX) src/main_runtime.cpp $(INCLUDE) -ldl -o program2

# # ----------------------------
# # linux targets (.so) — useful inside Docker
# # ----------------------------
# linux: linux_all

# linux_all: $(LIBDIR)/libcalc1.so $(LIBDIR)/libcalc2.so program1_linux program2_linux

# $(LIBDIR)/libcalc1.so:
# 	mkdir -p $(LIBDIR)
# 	$(CXX) $(CXXFLAGS) -shared src/derivative1.cpp src/e1.cpp -o $(LIBDIR)/libcalc1.so

# $(LIBDIR)/libcalc2.so:
# 	mkdir -p $(LIBDIR)
# 	$(CXX) $(CXXFLAGS) -shared src/derivative2.cpp src/e2.cpp -o $(LIBDIR)/libcalc2.so

# program1_linux:
# 	$(CXX) src/main_linking.cpp -L$(LIBDIR) -lcalc1 $(INCLUDE) -o program1_linux -Wl,-rpath,$(PWD)/$(LIBDIR)

# program2_linux:
# 	$(CXX) src/main_runtime.cpp $(INCLUDE) -ldl -o program2_linux

# # ----------------------------
# clean:
# 	rm -rf $(LIBDIR) program1 program2 program1_linux program2_linux

# # ----------------------------
# # Docker helpers
# # ----------------------------
# docker_build:
# 	docker build -t lab_trace .

# docker_run:
# 	docker run --rm -it -v $(PWD):/work -w /work lab_trace /bin/bash



# cmake_minimum_required(VERSION 3.10)
# project(DynLibDemo)

# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_BUILD_TYPE Release)

# # Указываем, где искать заголовки
# include_directories(include)

# # Основная программа
# add_executable(main_runtime src/main_runtime.cpp)
# target_link_libraries(main_runtime dl)

# # Библиотеки
# add_library(calc1 SHARED src/derivative1.cpp src/e1.cpp)
# add_library(calc2 SHARED src/derivative2.cpp src/e2.cpp)

# # Имена .so
# set_target_properties(calc1 PROPERTIES OUTPUT_NAME "calc1" PREFIX "lib")
# set_target_properties(calc2 PROPERTIES OUTPUT_NAME "calc2" PREFIX "lib")

# # Копируем .so в libs/
# add_custom_command(TARGET calc1 POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/libs
#     COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:calc1> ${CMAKE_BINARY_DIR}/libs/
# )
# add_custom_command(TARGET calc2 POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:calc2> ${CMAKE_BINARY_DIR}/libs/
# )


cmake_minimum_required(VERSION 3.10)
project(Lab4)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)

# Путь к заголовкам
include_directories(include)

# Библиотеки
add_library(calc1 SHARED src/derivative1.cpp src/e1.cpp)
add_library(calc2 SHARED src/derivative2.cpp src/e2.cpp)

# Имена .so
set_target_properties(calc1 PROPERTIES OUTPUT_NAME "calc1" PREFIX "lib")
set_target_properties(calc2 PROPERTIES OUTPUT_NAME "calc2" PREFIX "lib")

# main_linking — линкуется с libcalc1 (как program1)
add_executable(main_linking src/main_linking.cpp)
target_link_libraries(main_linking calc1)

# main_runtime — динамическая загрузка (как program2)
add_executable(main_runtime src/main_runtime.cpp)
target_link_libraries(main_runtime dl)

# После сборки копируем .so в build/libs/
add_custom_command(TARGET calc1 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/libs
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:calc1> ${CMAKE_BINARY_DIR}/libs/
)
add_custom_command(TARGET calc2 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:calc2> ${CMAKE_BINARY_DIR}/libs/
)