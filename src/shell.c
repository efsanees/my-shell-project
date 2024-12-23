#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  // fork ve execvp için gerekli
#include <sys/wait.h>  // wait için gerekli
#include "builtins.h"  // quit ve diğer komutlar için gerekli
#include "background.h" // Arkaplan komutları için gerekli
#include "redirect.h"  // Yönlendirme için gerekli

// Komutları işleyen fonksiyon
void handle_command(char *command) {
    // Argümanları ayır
    char *args[128];
    int i = 0;

    char *token = strtok(command, " ");
    while (token != NULL && i < 127) {
        args[i] = token;
        token = strtok(NULL, " ");
        i++;
    }
    args[i] = NULL;

    // Eğer giriş yönlendirmesi varsa
    if (strchr(command, '<')) {
        // Giriş yönlendirmesi işleme
        char *input_file = strtok(command, "<");
        input_file = strtok(NULL, " ");  // Dosya adı
        if (input_file) {
            input_redirect(input_file);  // Giriş dosyasını yönlendir
        } else {
            fprintf(stderr, "Giriş dosyası bulunamadı\n");
            return;
        }
    }

    // Eğer çıkış yönlendirmesi varsa
    if (strchr(command, '>')) {
        // Çıkış yönlendirmesi işleme
        char *output_file = strtok(command, ">");
        output_file = strtok(NULL, " ");  // Dosya adı
        if (output_file) {
            output_redirect(output_file);  // Çıkış dosyasını yönlendir
        } else {
            fprintf(stderr, "Çıkış dosyası açılamadı\n");
            return;
        }
    }

    // Arkaplan komutu kontrol et
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Çocuk süreç (komut çalıştır)
        if (execvp(args[0], args) == -1) {
            perror("execvp");
            exit(EXIT_FAILURE);
        }
    } else {
        // Ebeveyn süreç (çocuk süreci bekle)
        int status;
        waitpid(pid, &status, 0);
    }
}

// Main fonksiyonu
int main() {
    char input[1024];  // Komut girişi için tampon
    int status = 1;    // Kabuk çalıştığı sürece devam edecek

    while (status) {
        printf("> ");  // Komut istemi
        fflush(stdout);
        
        // Kullanıcıdan giriş al
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }

        // Yeni satır karakterini temizle
        input[strcspn(input, "\n")] = 0;

        // Komut analizini yap
        char *command = strtok(input, "\n");

        // NULL kontrolü
        if (command == NULL) {
            continue;
        }

        // Quit komutu
        if (strcmp(command, "quit") == 0) {
            printf("Çıkılıyor...\n");
            break;  // Döngüden çık
        }

        // Arkaplan komutları
        if (command[strlen(command) - 1] == '&') {
            command[strlen(command) - 1] = '\0';  // '&' karakterini kaldır
            handle_background(command);  // Arkaplan komutunu çalıştır
        } else {
            handle_command(command);  // Diğer komutları çalıştır
        }
    }

    return 0;
}
