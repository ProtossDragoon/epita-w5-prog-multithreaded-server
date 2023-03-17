# Compiler
CC = gcc

# Compiler flags
# -std=c99: C standard version
# -g: make debuggable
# -fsanitize=address: help to find memory leak
# -Wall: show all warnings
# -Wextra: show extra warnings
CFLAGS = -std=c99 -g -fsanitize=address -Wall -Wextra

# Linker flags
LDFLAGS = -fsanitize=address -pthread

# File variables
OBJ = test_shared_queue.o server.o shared_queue.o queue.o
DEP = ${OBJ:.o=.d}
EXC = test_shared_queue server

# Instruction
all: test_shared_queue server

test_shared_queue: test_shared_queue.o shared_queue.o queue.o

server: server.o shared_queue.o queue.o

shared_queue.o: shared_queue.c shared_queue.h queue.o

queue.o: queue.c queue.h

.PHONY: clean
clean:
	${RM} ${OBJ} ${DEP} ${EXC}
