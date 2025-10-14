#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s output_filename\n", argv[0]);
        return 1;
    }

    const char *outname = argv[1];
    FILE *out = fopen(outname, "a");
    if (!out) {
        perror("fopen output file");
        return 1;
    }

    char *line = NULL;
    size_t cap = 0;
    ssize_t n;

    while ((n = getline(&line, &cap, stdin)) != -1) {

        size_t len = (n > 0 && line[n-1] == '\n') ? (size_t)(n - 1) : (size_t)n;

        char original[1024];
        if (len >= sizeof(original)) len = sizeof(original) - 1;
        strncpy(original, line, len);
        original[len] = '\0';


        for (size_t i = 0, j = len - 1; i < j; ++i, --j) {
            char tmp = line[i];
            line[i] = line[j];
            line[j] = tmp;
        }


        printf("Transformed: %.*s\n", (int)len, line);
        fflush(stdout);


        fprintf(out, "Transformed: %.*s\n", (int)len, line);
        fflush(out);
    }

    free(line);
    fclose(out);
    return 0;
}
