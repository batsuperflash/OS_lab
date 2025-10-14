# Dockerfile для лабораторной: содержит компилятор, make и strace
FROM debian:stable-slim

RUN apt-get update && apt-get install -y \
    build-essential \
    gcc \
    make \
    strace \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /lab

CMD ["/bin/bash"]
