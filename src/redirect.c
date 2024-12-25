#include "redirect.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Giriş yönlendirmesi
void input_redirect(char *file) {
    int fd = open(file, O_RDONLY);
    if (fd == -1) {
        perror("Giriş dosyası bulunamadı");
        exit(EXIT_FAILURE);
    }
    dup2(fd, STDIN_FILENO);
    close(fd);
}

// Çıkış yönlendirmesi
void output_redirect(char *file) {
    int fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("Çıkış dosyası açılamadı");
        exit(EXIT_FAILURE);
    }
    dup2(fd, STDOUT_FILENO);
    close(fd);
}

// Komut çalıştırma
void execute_command_with_redirect(char *cmd, char *input_file, char *output_file) {
    pid_t pid = fork();

    if (pid == -1) {
        perror("Fork başarısız oldu");
        exit(1);
    }

    if (pid == 0) { // Çocuk işlem
        if (input_file != NULL) {
            input_redirect(input_file);
        }
        if (output_file != NULL) {
            output_redirect(output_file);
        }

        char *args[128];
        int i = 0;
        char *token = strtok(cmd, " ");
        while (token != NULL && i < 127) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;

        if (execvp(args[0], args) == -1) {
            perror("Komut çalıştırılamadı");
            exit(1);
        }
    } else {
        wait(NULL);
    }
}
