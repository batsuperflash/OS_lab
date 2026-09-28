# Dockerfile
FROM ubuntu:22.04

# Установка ВСЕХ необходимых зависимостей (исправлен синтаксис!)
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    pkg-config \
    libzmq3-dev \
    libzmq5 \
    strace \
    git \
    curl \
    vim \
    netcat \
    iputils-ping \
    procps \
    && rm -rf /var/lib/apt/lists/*

# Настройка рабочей директории
WORKDIR /app

# Копирование исходного кода
COPY server.cpp client.cpp common.hpp CMakeLists.txt ./

# Сборка проекта
RUN mkdir build && \
    cd build && \
    cmake .. && \
    make && \
    # Создание символических ссылок для удобства
    ln -s /app/build/server /usr/local/bin/server && \
    ln -s /app/build/client /usr/local/bin/client

# Порт для сервера
EXPOSE 5555

# Точка входа
CMD ["bash"]