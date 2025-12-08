# Контейнер нужен только для strace и для проверки linux-версии (.so).
FROM ubuntu:22.04

RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
      build-essential g++ make strace ca-certificates && \
    apt-get clean && rm -rf /var/lib/apt/lists/*

WORKDIR /work
COPY . /work

# Точка входа - оставляем bash, запуск вручную через docker_run
CMD ["/bin/bash"]
