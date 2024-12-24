#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

void execute_pipe(char *cmd1, char *cmd2) {
    int pipefd[2];  // Pipe için dosya tanımlayıcıları
    pid_t pid1, pid2;
    int status;

    // Pipe oluşturuluyor
    if (pipe(pipefd) == -1) {
        perror("Pipe oluşturulamadı");
        exit(EXIT_FAILURE);
    }

    // İlk komutun argümanlarını ayrıştır
    char *args1[128];
    int i = 0;
    char *token = strtok(cmd1, " ");
    while (token != NULL && i < 127) {
        args1[i] = token;
        token = strtok(NULL, " ");
        i++;
    }
    args1[i] = NULL;

    // İkinci komutun argümanlarını ayrıştır
    char *args2[128];
    i = 0;
    token = strtok(cmd2, " ");
    while (token != NULL && i < 127) {
        args2[i] = token;
        token = strtok(NULL, " ");
        i++;
    }
    args2[i] = NULL;

    // İlk komut için işlem oluşturuluyor
    if ((pid1 = fork()) == 0) {
        // Çocuk işlem (ilk komut)
        close(pipefd[0]);  // Okuma tarafını kapat
        dup2(pipefd[1], STDOUT_FILENO);  // Çıktıyı pipe'a yönlendir
        close(pipefd[1]);  // Pipe dosya tanımlayıcısını kapat
        if (execvp(args1[0], args1) == -1) {
            perror("execvp (ilk komut)");
            exit(EXIT_FAILURE);
        }
    } else if (pid1 < 0) {
        perror("Fork başarısız oldu (ilk komut)");
        exit(EXIT_FAILURE);
    }

    // İkinci komut için işlem oluşturuluyor
    if ((pid2 = fork()) == 0) {
        // Çocuk işlem (ikinci komut)
        close(pipefd[1]);  // Yazma tarafını kapat
        dup2(pipefd[0], STDIN_FILENO);  // Girdiyi pipe'dan al
        close(pipefd[0]);  // Pipe dosya tanımlayıcısını kapat
        if (execvp(args2[0], args2) == -1) {
            perror("execvp (ikinci komut)");
            exit(EXIT_FAILURE);
        }
    } else if (pid2 < 0) {
        perror("Fork başarısız oldu (ikinci komut)");
        exit(EXIT_FAILURE);
    }

    // Ebeveyn işlem, pipe dosya tanımlayıcılarını kapatıyor
    close(pipefd[0]);
    close(pipefd[1]);

    // Çocuk işlemleri bekleniyor
    waitpid(pid1, &status, 0);  // İlk komutun tamamlanmasını bekle
    if (WIFEXITED(status)) {
        printf("İlk işlem başarıyla tamamlandı. Çıkış kodu: %d\n", WEXITSTATUS(status));
    } else {
        printf("İlk işlem başarısız oldu.\n");
    }

    waitpid(pid2, &status, 0);  // İkinci komutun tamamlanmasını bekle
    if (WIFEXITED(status)) {
        printf("İkinci işlem başarıyla tamamlandı. Çıkış kodu: %d\n", WEXITSTATUS(status));
    } else {
        printf("İkinci işlem başarısız oldu.\n");
    }
}
