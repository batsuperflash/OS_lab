#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

static ssize_t write_all(int fd, const void *buf, size_t count) {
    const char *p = buf;
    size_t left = count;
    while (left > 0) {
        ssize_t w = write(fd, p, left);
        if (w < 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        left -= (size_t)w;
        p += w;
    }
    return (ssize_t)count;
}

int main(void) {
    char *fname1 = NULL, *fname2 = NULL;
    size_t fncap = 0;
    ssize_t r;

    printf("Enter filename for child1: ");
    fflush(stdout);
    r = getline(&fname1, &fncap, stdin);
    if (r <= 0) { perror("getline fname1"); free(fname1); return 1; }
    if (fname1[r-1] == '\n') fname1[r-1] = '\0';

    printf("Enter filename for child2: ");
    fflush(stdout);
    fncap = 0;
    r = getline(&fname2, &fncap, stdin);
    if (r <= 0) { perror("getline fname2"); free(fname1); free(fname2); return 1; }
    if (fname2[r-1] == '\n') fname2[r-1] = '\0';

    int p1[2], p2[2];
    if (pipe(p1) == -1) { perror("pipe p1"); free(fname1); free(fname2); return 1; }
    if (pipe(p2) == -1) { perror("pipe p2"); close(p1[0]); close(p1[1]); free(fname1); free(fname2); return 1; }

    pid_t pid1 = fork();
    if (pid1 < 0) {
        perror("fork pid1");
        close(p1[0]); close(p1[1]); close(p2[0]); close(p2[1]);
        free(fname1); free(fname2);
        return 1;
    }
    if (pid1 == 0) {
        if (dup2(p1[0], STDIN_FILENO) == -1) { perror("dup2 child1"); _exit(1); }
        close(p1[0]); close(p1[1]);
        close(p2[0]); close(p2[1]);
        execl("./child", "child", fname1, (char *)NULL);
        perror("execl child1");
        _exit(1);
    }

    close(p1[0]);


    pid_t pid2 = fork();
    if (pid2 < 0) {
        perror("fork pid2");

        kill(pid1, SIGTERM);
        close(p1[1]); close(p2[0]); close(p2[1]);
        free(fname1); free(fname2);
        return 1;
    }
    if (pid2 == 0) {

        if (dup2(p2[0], STDIN_FILENO) == -1) { perror("dup2 child2"); _exit(1); }
        close(p2[0]); close(p2[1]);
        close(p1[0]); close(p1[1]);
        execl("./child", "child", fname2, (char *)NULL);
        perror("execl child2");
        _exit(1);
    }

    close(p2[0]);


    signal(SIGPIPE, SIG_IGN);

    char *line = NULL;
    size_t linecap = 0;
    ssize_t linelen;
    long lineno = 0;

    printf("Enter lines (Ctrl-D to finish):\n");
    fflush(stdout);
    while ((linelen = getline(&line, &linecap, stdin)) != -1) {
        ++lineno;
        int target_fd = (lineno % 2 == 1) ? p1[1] : p2[1];
        if (write_all(target_fd, line, (size_t)linelen) == -1) {
            fprintf(stderr, "Error writing to pipe for child %s: %s\n",
                    (lineno % 2 == 1) ? "1" : "2", strerror(errno));
        }
    }

    free(line);


    close(p1[1]);
    close(p2[1]);

    int status;
    if (waitpid(pid1, &status, 0) == -1) perror("waitpid pid1");
    if (waitpid(pid2, &status, 0) == -1) perror("waitpid pid2");

    free(fname1);
    free(fname2);
    return 0;
}
