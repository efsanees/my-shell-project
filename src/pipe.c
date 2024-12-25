#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_COMMANDS 128
#define MAX_ARGS 128

// Boru komutlarını çalıştıran fonksiyon
void execute_pipe(char *command) {
    int pipefd[2];  // Pipe için dosya tanımlayıcıları
    pid_t pid;
    int status;

    // Komutları '|' ile ayırarak bir diziye alıyoruz
    char *commands[MAX_COMMANDS];
    int command_count = 0;
    char *token = strtok(command, "|");
    while (token != NULL && command_count < MAX_COMMANDS) {
        commands[command_count++] = token;
        token = strtok(NULL, "|");
    }

    int i;
    int prev_pipefd[2] = {-1, -1};  // Önceki pipe'ın dosya tanımlayıcıları

    // Her komut için işlemi fork ediyoruz
    for (i = 0; i < command_count; i++) {
        // Pipe oluşturuluyor
        if (i < command_count - 1) {  // Son komut dışında her zaman pipe oluştur
            if (pipe(pipefd) == -1) {
                perror("Pipe oluşturulamadı");
                exit(EXIT_FAILURE);
            }
        }

        // Komutun argümanlarını ayırıyoruz
        char *args[MAX_ARGS];
        int j = 0;
        char *cmd = commands[i];
        token = strtok(cmd, " ");
        while (token != NULL && j < MAX_ARGS - 1) {
            args[j++] = token;
            token = strtok(NULL, " ");
        }
        args[j] = NULL;

        // Fork ile her komut için yeni bir süreç oluşturuyoruz
        if ((pid = fork()) == 0) {
            // Eğer ilk komutsa veya önceki komuttan gelen çıktı varsa
            if (i > 0) {
                dup2(prev_pipefd[0], STDIN_FILENO);  // Önceki komuttan gelen çıktı
                close(prev_pipefd[0]);  // Önceki pipe'ı kapat
            }

            // Eğer son komut değilse, mevcut komutun çıktısını bir sonraki pipe'a yönlendiriyoruz
            if (i < command_count - 1) {
                dup2(pipefd[1], STDOUT_FILENO);  // Şu anki komutun çıktısını yeni pipe'a yönlendir
                close(pipefd[1]);  // Pipe dosya tanımlayıcısını kapat
            }

            // Komut çalıştırılıyor
            if (execvp(args[0], args) == -1) {
                perror("execvp hata");
                exit(EXIT_FAILURE);
            }
        } else if (pid < 0) {
            perror("Fork başarısız oldu");
            exit(EXIT_FAILURE);
        }

        // Şu anki komutun pipe'ı kapatılıyor
        if (i < command_count - 1) {
            close(pipefd[1]);
            prev_pipefd[0] = pipefd[0];  // Sonraki komut için pipefd[0]'ı sakla
        }
    }

    // Ebeveyn süreçlerin bitmesini bekliyoruz
    for (i = 0; i < command_count; i++) {
        wait(&status);
    }
}

int main() {
    char command[1024];

    while (1) {
        printf("ubuntu@ubuntu:~$ ");
        if (!fgets(command, sizeof(command), stdin)) {
            break;
        }

        command[strcspn(command, "\n")] = 0;  // Satır sonundaki yeni satır karakterini sil

        // 'quit' komutu girildiğinde shell'den çık
        if (strcmp(command, "quit") == 0) {
            break;
        }

        // Pipe komutunu çalıştır
        execute_pipe(command);
    }

    return 0;
}
