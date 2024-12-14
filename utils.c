#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#define MAX_ARGS 100 
// Komutları ayıklamak ve her birini argümanlara ayırmak için fonksiyon
void parse_command(char *input, char *args[]) {
    char *token = strtok(input, " ");
    int i = 0;
    
    while (token != NULL) {
        args[i++] = token;
        token = strtok(NULL, " ");
    }
    args[i] = NULL;
}

// Arka plan komutu kontrolü
int is_background_process(char *input) {
    // Komutun sonunda '&' olup olmadığını kontrol et
    if (input[strlen(input) - 1] == '&') {
        input[strlen(input) - 1] = '\0';  // '&' karakterini kaldır
        return 1;  // Arka planda çalışacak
    }
    return 0;  // Arka planda çalışmayacak
}

// Giriş yönlendirmesi (stdin) işlemi
int redirect_input(char *filename) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Giriş dosyası bulunamadı");
        return -1;
    }
    dup2(fd, STDIN_FILENO);  // stdin'i dosyaya yönlendir
    close(fd);
    return 0;
}

// Çıkış yönlendirmesi (stdout) işlemi
int redirect_output(char *filename) {
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("Çıkış dosyasına yazılamadı");
        return -1;
    }
    dup2(fd, STDOUT_FILENO);  // stdout'u dosyaya yönlendir
    close(fd);
    return 0;
}

// Pipe işlemini yapma
void handle_pipe(char *input) {
    int pipefd[2];
    pid_t pid1, pid2;

    // Boru oluştur
    pipe(pipefd);

    // İlk komutun çalıştırılması
    pid1 = fork();
    if (pid1 == 0) {
        // İlk komut
        char *args1[MAX_ARGS];
        char *first_command = strtok(input, "|");
        parse_command(first_command, args1);
        dup2(pipefd[1], STDOUT_FILENO);  // Çıktıyı pipe'a yönlendir
        close(pipefd[0]);
        execvp(args1[0], args1);
        perror("execvp hatası");
        exit(1);
    }

    // İkinci komutun çalıştırılması
    pid2 = fork();
    if (pid2 == 0) {
        // İkinci komut
        char *args2[MAX_ARGS];
        char *second_command = strtok(NULL, "|");
        parse_command(second_command, args2);
        dup2(pipefd[0], STDIN_FILENO);  // Girdiyi pipe'tan al
        close(pipefd[1]);
        execvp(args2[0], args2);
        perror("execvp hatası");
        exit(1);
    }

    // Ana süreç boruyu kapatır ve çocukların bitmesini bekler
    close(pipefd[0]);
    close(pipefd[1]);
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
}
