CC = gcc
CFLAGS = -Wall
OBJS = Shell.o command_things.o lsh.o builtin.o feature.o
TARGET = Shell

.SUFFIXES: .c .o

all : $(TARGET)

$(TARGET) : $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

Shell.o : Shell.c

command_things.o : command_things.c constants.h

lsh.o : lsh.c lsh.h constants.h command_things.h builtin.h feature.h

builtin.o : builtin.c builtin.h

feature.o : feature.c feature.h

clean:
	rm -f $(OBJS) $(TARGET)
