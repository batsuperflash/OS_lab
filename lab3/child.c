#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

struct Shared {
    volatile sig_atomic_t ready;  // 0 - пусто, 1 - данные готовы
    char buffer[1024];
};

volatile sig_atomic_t got_signal = 0;

void sigusr1_handler(int sig) {
    got_signal = 1;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s mmap_filename\n", argv[0]);
        return 1;
    }

    const char *fname = argv[1];
    int fd = open(fname, O_RDWR);
    if (fd == -1) {
        perror("open mmap file");
        return 1;
    }

    struct Shared *shared = mmap(NULL, sizeof(struct Shared),
                                 PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shared == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return 1;
    }

    close(fd);

    // установка обработчика сигнала
    struct sigaction sa = {0};
    sa.sa_handler = sigusr1_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction");
        return 1;
    }

    pid_t parent_pid = getppid();

    while (1) {
        pause();  // ждем сигнала SIGUSR1
        if (!got_signal) continue;
        got_signal = 0;

        if (shared->ready == 0) continue;

        // обработка строки - реверс
        size_t len = strnlen(shared->buffer, sizeof(shared->buffer));
        for (size_t i = 0, j = len - 1; i < j; ++i, --j) {
            char tmp = shared->buffer[i];
            shared->buffer[i] = shared->buffer[j];
            shared->buffer[j] = tmp;
        }

        shared->ready = 2;  // данные обработаны

        kill(parent_pid, SIGUSR2);  // уведомление родителя
    }

    munmap(shared, sizeof(struct Shared));
    return 0;
}
