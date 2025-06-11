CC = gcc
LIBS = -pthread
CFLAGS = -Wall -Wextra
OBJS = main.o event.o manager.o resource.o system.o 
EXECS = p2

%.o: %.c defs.h
		$(CC) -c $< -o $@ $(CFLAGS)

$(EXECS): $(OBJS)
		$(CC) $(OBJS) -o $(EXECS) $(LIBS)

clean:
		rm -f $(OBJS) $(EXECS)

