CC = gcc
CFLAGS = -Wall
TARGET = Shell
OBJECTS = ../src/Shell.o ../src/command_things.o ../src/lsh.o

.SUFFIXES: .c .o

all : $(TARGET)

$(TARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS)

Shell.o : 			../src/Shell.c

command_things.o : 	../src/command_things.c ../src/constants.h

lsh.o : 			../src/lsh.c ../src/constants.h ../src/command_things.h

clean :
	rm $(TARGET)
	rm ../src/*.o
