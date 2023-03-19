#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define readpipe "/tmp/readpipe"
#define writepipe "/tmp/writepipe"

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

    char text[10000] = {};
    char text_buf[10000] = {};

    mknod(readpipe, S_IFIFO | 0666, 0);
    mknod(writepipe, S_IFIFO | 0666, 0);

    pid_t is_reader = fork();

    if (is_reader) {
        printf("Reader process online\n");

        int read_fd = open(argv[1], O_RDONLY);
        read(read_fd, text, sizeof(text));

        int read_pipe = open(readpipe, O_WRONLY);

        write(read_pipe, text, sizeof(text));

        close(read_fd);
        close(read_pipe);

        int write_pipe = open(writepipe, O_RDONLY);
        read(write_pipe, text_buf, sizeof(text_buf));
        int write_fd = open(argv[2], O_WRONLY | O_CREAT, 0666);
        write(write_fd, text_buf, strlen(text_buf));
        close(write_fd);
        close(write_pipe);
        unlink(readpipe);
        unlink(writepipe);
    } else {
        printf("Processor process online\n");

        int read_pipe = open(readpipe, O_RDONLY);
        int write_pipe = open(writepipe, O_WRONLY);
        read(read_pipe, text, sizeof(text));

        printf("Processor reads and transforms string\n");

        toup(text);
        write(write_pipe, text, sizeof(text));

        close(read_pipe);
        close(write_pipe);
    }

    printf("Processor finished work\n");

    return 0;
}