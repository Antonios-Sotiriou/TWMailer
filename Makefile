#############################################################################################
# Makefile
#############################################################################################

CC=gcc

#############################################################################################
# -g: produces debugging information (for gdb)
# -Wall: enables all the warnings
# -Wextra: further warnings
# -Werror: treat warnings as errors
# -O: Optimizer turned on
# -c: says not to run the linker
# -pthread: Add support for multithreading using the POSIX threads library. This option sets 
#           flags for both the preprocessor and linker. It does not affect the thread safety 
#           of object code produced by the compiler or that of libraries supplied with it. 
#           These are HP-UX specific flags.
#############################################################################################
CFLAGS=-g -Wall -Wextra -Werror -O -pthread

rebuild: clean all
all: ./bin/server ./bin/client

clean:
	clear
	rm -f bin/* obj/*

./obj/client.o: client.c
	${CC} ${CFLAGS} -o obj/client.o client.c -c

./obj/server.o: server.c
	${CC} ${CFLAGS} -o obj/server.o server.c -c

./obj/general_funcs.o: source/general_funcs.c
	${CC} ${CFLAGS} -o obj/general_funcs.o source/general_funcs.c -c 

./bin/server: ./obj/server.o ./obj/general_funcs.o
	${CC} ${CFLAGS} -o bin/server obj/server.o ./obj/general_funcs.o

./bin/client: ./obj/client.o
	${CC} ${CFLAGS} -o bin/client obj/client.o

