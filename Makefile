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
