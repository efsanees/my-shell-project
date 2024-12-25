#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

// Arka plan süreçlerin takibi için global liste
pid_t background_processes[128];
int process_count = 0;

// Arka plan süreçlerin sona ermesini izleyen sinyal işleyici
void background_process_handler(int sig) {
    int status;
    pid_t pid;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        printf("[%d] retval: %d\n", pid, WEXITSTATUS(status));
        // Süreci listeden çıkar
        for (int i = 0; i < process_count; i++) {
            if (background_processes[i] == pid) {
                background_processes[i] = background_processes[--process_count];
                break;
            }
        }
    }
}

// Arka planda komut çalıştırma
void handle_background(char *command) {
    command[strlen(command) - 1] = '\0'; // '&' karakterini kaldır

    char *args[128];
    int i = 0;

    char *token = strtok(command, " ");
    while (token != NULL && i < 127) {
        args[i++] = token;
        token = strtok(NULL, " ");
    }
    args[i] = NULL;

    pid_t pid = fork(); // Çocuk işlem oluştur

    if (pid == -1) {
        perror("Fork başarısız oldu");
        return;
    }

    if (pid == 0) { // Çocuk işlem
        if (execvp(args[0], args) == -1) {
            perror(args[0]);
            exit(EXIT_FAILURE);
        }
    } else { // Ebeveyn işlem
        background_processes[process_count++] = pid;
        printf("[%d] Arka planda çalıştırılıyor...\n", pid);
    }
}

// Tüm arka plan süreçleri bekle
void wait_for_all_background_processes() {
    for (int i = 0; i < process_count; i++) {
        int status;
        waitpid(background_processes[i], &status, 0);
        printf("[%d] retval: %d\n", background_processes[i], WEXITSTATUS(status));
    }
}
