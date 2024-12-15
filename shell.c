#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "utils.h"

#define MAX_CMD_LENGTH 1024
#define MAX_ARGS 100

// Komutları ayıklamak ve çalıştırmak için yardımcı fonksiyon
void execute_command(char *input) {
    char *args[MAX_ARGS];
    int background = 0;

    // Komut arka planda mı çalışacak kontrol et
    background = is_background_process(input);

    // Komutları ayıkla
    parse_command(input, args);

    // Giriş yönlendirme (input < dosya)
    if (strstr(input, "<") != NULL) {
        char *input_file = strtok(input, "<");
        strtok(NULL, " ");  // boşluğu atla
        char *file_name = strtok(NULL, " ");
        if (redirect_input(file_name) == -1) {
            return;
        }
    }

    // Çıkış yönlendirme (output > dosya)
    if (strstr(input, ">") != NULL) {
        char *output_file = strtok(input, ">");
        strtok(NULL, " ");  // boşluğu atla
        char *file_name = strtok(NULL, " ");
        if (redirect_output(file_name) == -1) {
            return;
        }
    }

    // Boru (pipe) işlemleri için
    if (strstr(input, "|") != NULL) {
        handle_pipe(input);
        return;
    }

    // Komut çalıştırma
    pid_t pid = fork();
    if (pid == 0) {
        // Çocuk süreç
        if (execvp(args[0], args) == -1) {
            perror("execvp hatası");
            exit(1);
        }
    } else if (pid < 0) {
        perror("fork hatası");
        return;
    }

    // Eğer arka planda çalışmayacaksa, komutun bitmesini bekle
    if (!background) {
        int status;
        waitpid(pid, &status, 0);
    } else {
        // Arka planda çalışıyorsa, pid ve exit kodunu yazdır
        printf("[%d] running in background\n", pid);
    }
}

// Quit komutunun işlemi burada yapılıyor
void quit_shell() {
    printf("Çıkılıyor...\n");
    exit(0);
}

int main() {
    char input[MAX_CMD_LENGTH];

    while (1) {
        printf("> ");
        fflush(stdout);

        // Komutları oku
        if (!fgets(input, MAX_CMD_LENGTH, stdin)) {
            perror("fgets hatası");
            break;
        }

        // Yeni satır karakterini temizle
        input[strcspn(input, "\n")] = 0;

        // Quit komutunu kontrol et
        if (strcmp(input, "quit") == 0) {
            quit_shell();
        }

        // Komutu çalıştır
        execute_command(input);
    }

    return 0;
}