CC = gcc
CFLAGS = -Wall
OBJS = Shell.o command_things.o lsh.o builtin.o
TARGET = Shell

.SUFFIXES: .c .o

all : $(TARGET)

$(TARGET) : $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

Shell.o : Shell.c

command_things.o : command_things.c constants.h

lsh.o : lsh.c constants.h

builtin.o : builtin.o

clean:
	rm -f $(OBJS) $(TARGET)
