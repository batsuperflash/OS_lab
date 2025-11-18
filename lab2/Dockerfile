FROM ubuntu:20.04
RUN apt-get update && apt-get install -y gcc strace
WORKDIR /app
COPY . ./
RUN gcc -pthread -o convolution *.c -lm 
CMD ["./convolution", "4", "500", "3", "2"]


