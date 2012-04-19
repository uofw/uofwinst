CC = gcc
OBJ = main.o
LIBS = -lz
TARGET = psp_packer
RM = rm -f

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LIBS)

main.o: main.c
	$(CC) -c main.c -o main.o

clean:
	${RM} $(OBJ) $(TARGET)

