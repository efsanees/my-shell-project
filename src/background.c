#include "background.h"
#include "utils.h"
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>  // PID türü için eklenmeli
#include <sys/wait.h>   // waitpid için eklenmeli

void handle_background(char *command) {
    // '&' karakterini komuttan kaldır
    command[strlen(command) - 1] = '\0';

    // Komutu ve argümanlarını ayrıştır
    char *args[128];
    int i = 0;

    char *token = strtok(command, " ");
    while (token != NULL && i < 127) {
        args[i] = token;
        token = strtok(NULL, " ");
        i++;
    }
    if (i >= 127) {
        fprintf(stderr, "Hata: Çok fazla argüman.\n");
        return;
    }
    args[i] = NULL;  // Argüman dizisinin sonunu belirt

    pid_t pid = fork();  // Yeni bir işlem (child process) oluştur

    if (pid == -1) {
        perror("Fork başarısız oldu");
        return;
    }

    if (pid == 0) {  // Çocuk işlemi
        // Komutu çalıştır
        if (execvp(args[0], args) == -1) {
            perror(args[0]);  // Komut hatası ile birlikte belirtilir
            exit(1);
        }
    } else {  // Ebeveyn işlemi
        printf("Arka planda %s komutu çalıştırılıyor... [PID: %d]\n", args[0], pid);

        // Arka plan işleminin durumunu izlemek için isteğe bağlı kod
        int status;
        waitpid(pid, &status, WNOHANG);  // Beklemeden durum kontrolü
    }
}
