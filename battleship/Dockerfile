# Используем официальный образ Ubuntu
FROM ubuntu:22.04

# Не задаём интерактивный режим (чтобы apt не спрашивал)
ENV DEBIAN_FRONTEND=noninteractive

# Устанавливаем компилятор, CMake, ZeroMQ и strace
RUN apt-get update && \
    apt-get install -y g++ cmake libzmq3-dev pkg-config make strace && \
    rm -rf /var/lib/apt/lists/*

# Создаём рабочую директорию
WORKDIR /app

# Копируем все файлы проекта
COPY . .

# Собираем проект
RUN mkdir -p build && cd build && cmake .. && make

# Открываем порт для ZeroMQ
EXPOSE 5555

# По умолчанию запускаем bash
CMD ["/bin/bash"]
