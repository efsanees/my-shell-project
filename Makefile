# Derleyici ve bayrakları tanımlıyoruz
CC = gcc
CFLAGS = -Wall -g -I./include   # Include dizinini burada belirtiyoruz

# Kaynak dosyalarını ve başlık dosyalarını belirtin
SRC = src/shell.c src/builtins.c src/redirect.c src/pipe.c src/background.c src/utils.c
OBJ = $(SRC:.c=.o)
EXEC = shell

# Varsayılan hedef (Makefile'i çalıştırdığınızda bu hedef çalışır)
all: $(EXEC)

# İlgili hedef: shell programını oluşturmak için gerekli olan adımlar
$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC)

# Kaynak dosyalarının .o dosyalarına dönüştürülmesi
src/shell.o: src/shell.c include/shell.h
	$(CC) $(CFLAGS) -c src/shell.c -o src/shell.o

src/builtins.o: src/builtins.c include/builtins.h
	$(CC) $(CFLAGS) -c src/builtins.c -o src/builtins.o

src/redirect.o: src/redirect.c include/redirect.h   # redirect.h dosyasına referans
	$(CC) $(CFLAGS) -c src/redirect.c -o src/redirect.o

src/pipe.o: src/pipe.c include/pipe.h
	$(CC) $(CFLAGS) -c src/pipe.c -o src/pipe.o

src/background.o: src/background.c include/background.h  # background.h dosyasına referans
	$(CC) $(CFLAGS) -c src/background.c -o src/background.o

src/utils.o: src/utils.c include/utils.h
	$(CC) $(CFLAGS) -c src/utils.c -o src/utils.o

# Temizlik: Derleme sırasında oluşan geçici dosyaları siler
clean:
	rm -f src/*.o $(EXEC)

# Yardımcı hedef (opsiyonel)
.PHONY: all clean
