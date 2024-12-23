#include "builtins.h"
#include <stdio.h>
#include <stdlib.h>

// Quit komutunun işlevini yerine getiren fonksiyon
void quit() {
    printf("Çıkılıyor...\n");
    exit(0);  // Programdan çıkılır
}
