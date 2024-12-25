CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = shell
SRC = src/shell.c
OBJ = shell.o

# Derleme hedefi
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

# Objeleri oluştur
$(OBJ): src/shell.c # Her iki dosyayı da burada belirtin
	$(CC) $(CFLAGS) -c src/shell.c

# Temizlik
clean:
	rm -f $(OBJ) $(TARGET)

# Çalıştırma
run: $(TARGET)
	./$(TARGET)
