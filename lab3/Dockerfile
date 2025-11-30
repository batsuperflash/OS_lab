FROM debian:stable-slim

# Установка компилятора и strace для отладки
RUN apt-get update && apt-get install -y \
    build-essential \
    strace \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /lab

# Копируем исходники и компилируем
COPY parent.c child.c ./
RUN gcc -std=c11 -O2 -Wall -o parent parent.c \
    && gcc -std=c11 -O2 -Wall -o child child.c

# По умолчанию запускаем bash
CMD ["/bin/bash"]
