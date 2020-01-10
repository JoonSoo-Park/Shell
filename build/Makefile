
CC = gcc
CFLAGS = -Wall
TARGET = Shell
OBJECTS = Shell.o

$(TARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $^

Shell.o : ../src/Shell.c
	$(CC) $(CFLAGS) -c -o Shell.o ../src/Shell.c

clean :
	rm *.o $(TARGET)
