#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>

#define MAX_COMMAND_SIZE 1024
#define MAX_ARGS 128

// Giriş yönlendirmesi
void input_redirect(const char *file_name) {
    int input_fd = open(file_name, O_RDONLY);
    if (input_fd == -1) {
        perror("Dosya açılamadı");
        exit(EXIT_FAILURE);
    }
    dup2(input_fd, STDIN_FILENO);  // stdin'e yönlendir
    close(input_fd);
}

// Çıkış yönlendirmesi
void output_redirect(const char *file_name) {
    int output_fd = open(file_name, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (output_fd == -1) {
        perror("Dosya açılamadı");
        exit(EXIT_FAILURE);
    }
    dup2(output_fd, STDOUT_FILENO);  // stdout'a yönlendir
    close(output_fd);
}

// Arka plan işlemlerinin sonlanmasını bekle
void wait_for_background_processes() {
    int status;
    pid_t pid;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        if (WIFEXITED(status)) {
            int exit_code = WEXITSTATUS(status);
            printf("[%d] retval: %d\n", pid, exit_code);
        }
    }
}

// Komutları çalıştıran ana fonksiyon
void handle_command(char *command) {
    char *args[MAX_ARGS];
    int i = 0;
    int background = 0;

    // Komutları ayır
    char *token = strtok(command, " ");
    while (token != NULL && i < MAX_ARGS - 1) {
        args[i] = token;
        token = strtok(NULL, " ");
        i++;
    }
    args[i] = NULL;

    // Eğer 'cat' komutu ise ve giriş yönlendirmesi varsa, dosyayı oku
    if (strchr(command, '<')) {
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
        char *output_file = strtok(command, ">");
        output_file = strtok(NULL, " ");  // Dosya adı
        if (output_file) {
            output_redirect(output_file);  // Çıkış dosyasını yönlendir
        } else {
            fprintf(stderr, "Çıkış dosyası açılamadı\n");
            return;
        }
    }

    // Arka planda çalışıp çalışmadığını kontrol et
    if (args[i - 1] != NULL && strcmp(args[i - 1], "&") == 0) {
        background = 1;
        args[i - 1] = NULL;  // Arka planda çalışıyorsa '&'i kaldır
    }

    // Komut çalıştırılacak
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Çocuk süreç: komut çalıştırma
        if (execvp(args[0], args) == -1) {
            perror("execvp");
            exit(EXIT_FAILURE);
        }
    } else {
        if (background) {
            // Arka planda çalışacaksa PID'yi hemen yazdır
            printf("[%d] %s\n", pid, command);
        } else {
            // Çocuk süreç bitene kadar bekle
            int status;
            waitpid(pid, &status, 0);
            if (WIFEXITED(status)) {
                int exit_code = WEXITSTATUS(status);
                printf("[%d] retval: %d\n", pid, exit_code);
            }
        }
    }
    

    }

// Ana shell fonksiyonu
int main() {
    char command[MAX_COMMAND_SIZE];

    while (1) 
    {
        
        printf("\n");
        // Her komut öncesinde prompt'u yazdır
        printf("ubuntu@ubuntu:~/my-shell-project$ > ");
        
        // Kullanıcıdan komut al
        if (!fgets(command, sizeof(command), stdin)) {
            break;  // Eğer giriş okuma hatası olursa çık
        }

        // Satır sonundaki yeni satır karakterini sil
        command[strcspn(command, "\n")] = 0;

        // Eğer "quit" komutu yazılırsa shell'den çık
        if (strcmp(command, "quit") == 0) {
            wait_for_background_processes();  // Arka plan işlemleri sonlandırılacak
            break;
        }

        // Komutu işle
        handle_command(command);

        // Arka planda çalışan süreçleri bekle
        wait_for_background_processes();
    }

    return 0;
}
