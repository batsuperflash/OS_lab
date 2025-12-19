FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get install -y g++ cmake libzmq3-dev pkg-config make strace && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

RUN mkdir -p build && cd build && cmake .. && make

EXPOSE 5555

CMD ["/bin/bash"]
