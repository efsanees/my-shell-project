#
#EFSANE SİNANOĞLU G221210034 1-A
#MELİKE GÜN B221210089 1-B
#ALPER ZEYBEK B221210025 1-A
#MUSTAFA ALPEREN AKÇA B221210014 1-B
#SEFA MALKOÇ B221210043 1-A
#

CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = shell
SRC = src/shell.c
OBJ = shell.o

# Derleme hedefi
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

# Objeleri oluştur
$(OBJ): $(SRC)
	$(CC) $(CFLAGS) -c $(SRC) -o $(OBJ)

# Temizlik
clean:
	rm -f $(OBJ) $(TARGET)

# Çalıştırma
run: $(TARGET)
	./$(TARGET)

# Header dosyaları için otomatik bağımlılık
%.o: %.c %.h
