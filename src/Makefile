CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -pedantic -D_POSIX_C_SOURCE=200809L

all: parent child

parent: parent.c
	$(CC) $(CFLAGS) parent.c -o parent

child: child.c
	$(CC) $(CFLAGS) child.c -o child

clean:
	rm -f parent child
.PHONY: all clean


run:
	docker run --rm -it \
		--cap-add=SYS_PTRACE --security-opt seccomp=unconfined \
		-v "$(PWD)":/lab -w /lab lab-processes:latest \
		bash -c "make && strace -f -e trace=process,pipe,execve,waitpid ./parent"
