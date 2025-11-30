#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>

struct Shared {
    volatile sig_atomic_t ready;
    char buffer[1024];
};

volatile sig_atomic_t child_done = 0;

void sigusr2_handler(int sig) {
    child_done = 1;
}

int main(void) {
    char *fname = "shared.dat";

    int fd = open(fname, O_RDWR | O_CREAT, 0666);
    if (fd == -1) { perror("open"); return 1; }
    if (ftruncate(fd, sizeof(struct Shared)) == -1) { perror("ftruncate"); close(fd); return 1; }

    struct Shared *shared = mmap(NULL, sizeof(struct Shared),
                                 PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shared == MAP_FAILED) { perror("mmap"); close(fd); return 1; }

    close(fd);

    shared->ready = 0;

    // установка сигнала для обработки завершения работы детей
    struct sigaction sa = {0};
    sa.sa_handler = sigusr2_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGUSR2, &sa, NULL) == -1) { perror("sigaction"); return 1; }

    pid_t pid1 = fork();
    if (pid1 < 0) { perror("fork"); return 1; }
    if (pid1 == 0) { execl("./child", "child", fname, NULL); perror("execl"); _exit(1); }

    pid_t pid2 = fork();
    if (pid2 < 0) { perror("fork"); kill(pid1, SIGTERM); return 1; }
    if (pid2 == 0) { execl("./child", "child", fname, NULL); perror("execl"); _exit(1); }

    printf("Enter lines (Ctrl-D to finish):\n");

    char *line = NULL;
    size_t cap = 0;
    ssize_t n;

    long lineno = 0;
    while ((n = getline(&line, &cap, stdin)) != -1) {
        ++lineno;

        // пишем строку в shared memory
        strncpy(shared->buffer, line, sizeof(shared->buffer) - 1);
        shared->buffer[sizeof(shared->buffer)-1] = '\0';
        shared->ready = 1;

        // сигналим нужному ребенку
        pid_t target = (lineno % 2 == 1) ? pid1 : pid2;
        kill(target, SIGUSR1);

        // ждём пока child обработает
        while (!child_done) pause();
        child_done = 0;

        printf("Processed: %s\n", shared->buffer);
    }

    free(line);

    // завершение детей
    kill(pid1, SIGTERM);
    kill(pid2, SIGTERM);

    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    munmap(shared, sizeof(struct Shared));
    return 0;
}
