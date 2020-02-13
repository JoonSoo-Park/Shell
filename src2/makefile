CC = gcc
CFLAGS = -Wall
LDLIBS = -lreadline -ggdb
OBJS = shell.o
TARGET = shell

.SUFFIXED: .c .o

all : $(TARGET)

$(TARGET) : $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDLIBS)

clean :
	rm -f $(OBJS) $(TARGET)
