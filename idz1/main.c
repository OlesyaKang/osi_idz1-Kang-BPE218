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

    int rp[2], wp[2];
    char text[10000] = {};

    pipe(rp);
    pipe(wp);

    pid_t is_reader = fork();

    if (is_reader) {
        printf("Reader processes online\n");
        int read_fd = open(argv[1], O_RDONLY);
        read(read_fd, text, sizeof(text));

        write(rp[1], text, sizeof(text));

        close(read_fd);
    } else {
        pid_t is_writer = fork();

        if (is_writer) {
            printf("Writer processes online\n");
            read(wp[0], text, sizeof(text));

            int write_fd = open(argv[2], O_WRONLY | O_CREAT, 0666);

            write(write_fd, text, strlen(text));

            close(write_fd);
        } else {
            printf("Processor processes online\n");

            read(rp[0], text, sizeof(text));

            printf("Processor reads and transforms string\n");

            toup(text);

            write(wp[1], text, sizeof(text));
        }
    }

    close(rp[0]);
    close(rp[1]);
    close(wp[0]);
    close(wp[1]);

    printf("Processor finished work\n");

    return 0;
}