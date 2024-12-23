#include "redirect.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

void input_redirect(char *file) {
    int fd = open(file, O_RDONLY);  // Dosyayı okuma modunda aç
    if (fd == -1) {  // Dosya açılamazsa, hata mesajı ver
        perror("Giriş dosyası bulunamadı");
        exit(1);
    }
    dup2(fd, STDIN_FILENO);  // Standart girişi (stdin) dosyaya yönlendir
    close(fd);  // Dosya tanımlayıcısını kapat
}


// Çıkış yönlendirmesi: komutun çıktısını dosyaya yönlendirir
void output_redirect(char *file) {
    int fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644);  // Dosyayı yazma modunda aç veya oluştur
    if (fd == -1) {
        perror("Çıkış dosyası açılamadı");
        exit(1);
    }
    dup2(fd, STDOUT_FILENO);  // Standart çıktıyı dosyaya yönlendir
    close(fd);
}


// Yönlendirme ile komut çalıştırma
void execute_command_with_redirect(char *cmd, char *input_file, char *output_file) {
    pid_t pid = fork();  // Yeni bir çocuk işlem oluştur

    if (pid == -1) {
        perror("Fork başarısız oldu");
        exit(1);
    }

    if (pid == 0) {  // Çocuk işlemde
        if (input_file != NULL) {
            input_redirect(input_file);  // Giriş yönlendirmesini uygula
        }
        if (output_file != NULL) {
            output_redirect(output_file);  // Çıkış yönlendirmesini uygula
        }

        // Komutları çalıştır
        char *args[128];
        int i = 0;
        char *token = strtok(cmd, " ");
        while (token != NULL && i < 127) {
            args[i] = token;
            token = strtok(NULL, " ");
            i++;
        }
        args[i] = NULL;

        if (execvp(args[0], args) == -1) {
            perror("Komut çalıştırılamadı");
            exit(1);
        }
    } else {  // Ebeveyn işlemde
        wait(NULL);  // Çocuk işlem tamamlanana kadar bekle
    }
}
