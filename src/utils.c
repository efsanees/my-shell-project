#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void parse_command(char *command) {
    char *cmd = strdup(command);  // Komutun bir kopyasını oluşturuyoruz
    char *token = strtok(cmd, " ");  // Komutu parçalamak için ilk token'ı alıyoruz

    // İlk token'ı (yani komutun adı) işle
    if (token != NULL) {
        // Bu noktada token, kullanıcının girdiği komutun ilk kelimesi oluyor
        printf("Komut: %s\n", token);

        // Diğer token'ları işlemek
        token = strtok(NULL, " ");
        while (token != NULL) {
            printf("Parametre: %s\n", token);
            token = strtok(NULL, " ");
        }
    }

    free(cmd);  // Bellek sızıntısı olmaması için ayrılan belleği serbest bırakıyoruz
}
