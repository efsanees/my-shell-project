CC = gcc
CFLAGS = -Wall -std=c99 -g

# Kaynak dosyalar
SRC = shell.c utils.c

# Çıktı dosyası
OUT = shell

# Derleme komutları
all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) -o $(OUT) $(SRC)

# Temizlik komutları
clean:
	rm -f $(OUT)
