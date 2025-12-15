# # Контейнер нужен только для strace и для проверки linux-версии (.so).
# FROM ubuntu:22.04

# RUN apt-get update && \
#     DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
#       build-essential g++ make strace ca-certificates && \
#     apt-get clean && rm -rf /var/lib/apt/lists/*

# WORKDIR /work
# COPY . /work

# # Точка входа - оставляем bash, запуск вручную через docker_run
# CMD ["/bin/bash"]

# # lab4/Dockerfile
# FROM ubuntu:22.04

# WORKDIR /work

# # Установка зависимостей
# RUN apt-get update && \
#     apt-get install -y g++ cmake make strace && \
#     rm -rf /var/lib/apt/lists/*

# # Копируем ВСЁ из текущей директории (lab4/) в контейнер:
# # - CMakeLists.txt
# # - src/
# # - include/
# # - и т.д.
# COPY . .

# # Сборка
# RUN mkdir build && cd build && \
#     cmake .. && \
#     make -j$(nproc)

# # Подготовка к запуску: копируем исполняемый файл и libs/
# RUN cp build/main_runtime . && \
#     cp -r build/libs .

# CMD ["./main_runtime"]



FROM ubuntu:22.04

WORKDIR /work

# Установка зависимостей
RUN apt-get update && \
    apt-get install -y g++ cmake make strace && \
    rm -rf /var/lib/apt/lists/*

# Копируем всё
COPY . .

# Сборка
RUN mkdir build && cd build && \
    cmake .. && \
    make

# Готовим к запуску: копируем исполняемые файлы и libs/
RUN cp build/main_linking build/main_runtime . && \
    cp -r build/libs .

# По умолчанию ничего не запускаем — пользователь сам выберет
CMD ["echo", "Available: ./main_linking or ./main_runtime"]