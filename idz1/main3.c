#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void toup(char *str) {
    int l = strlen(str);
    for (int i = 0; i < l; i++) {
        if (strchr("bcdfghjklmnpqrstvwxz", str[i]) != NULL) {
            str[i] -= 32;
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Invalid argument number\n");
        return 1;
    }

    int rp, wp;
    char text[10000] = {};

    pipe(rp);
    pipe(wp);

    pid_t is_reader = fork();

    if (is_reader) {
        printf("Reader processes online\n");
        int read_fd = open(argv[1], O_RDONLY);
        read(read_fd, text, sizeof(text));

        write(rp, text, sizeof(text));

        close(read_fd);

        printf("Writer processes online\n");
        read(wp, text, sizeof(text));

        int write_fd = open(argv[2], O_WRONLY | O_CREAT, 0666);

        write(write_fd, text, strlen(text));

        close(write_fd);
    } else {
        printf("Processor processes online\n");

        read(rp, text, sizeof(text));

        printf("Processor reads and transforms string\n");

        toup(text);

        write(wp, text, sizeof(text));
    }

    close(rp);
    close(rp);

    printf("Processor finished work\n");

    return 0;
}