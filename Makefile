# @file makefile
# @author Sniehovskyi Nikita (xsnieh00)
# @date 11.04.2024
# @brief Project builder. Use 'make' to create default server

CC = gcc
CFLAGS = -Wall -Wextra --std=c11

EXEC = ipk24chat-server

ZIP_NAME = "xsnieh00.zip"

SRCDIR = src/
OBJDIR = objs/

# HEADS = $(wildcard $(SRCDIR)*.h)
HEADS = src/main.h src/server.h src/error.h src/argv.h src/types.h src/queue.h src/utils.h src/parse.h
# SRCS = $(wildcard $(SRCDIR)*.c)
SRCS = src/main.c src/server.c src/error.c src/argv.c src/types.c src/queue.c src/utils.c src/parse.c
# OBJS = $(patsubst $(SRCDIR)%.c,$(OBJDIR)%.o,$(SRCS))
OBJS = objs/main.o objs/server.o objs/error.o objs/argv.o objs/types.o objs/queue.o objs/utils.o objs/parse.o

.PHONY: all

all: clear build

run: clear build
	./$(EXEC)

build: mkobjdir $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

$(OBJDIR)%.o: $(SRCDIR)%.c
	$(CC) $(CFLAGS) $^ -r -o $@

mkobjdir:
	mkdir -p $(OBJDIR)

clear:
	rm -rf $(OBJDIR)
	rm -f $(ZIP_NAME)
	rm -f $(EXEC)

zip:
	rm -f $(ZIP_NAME)
	zip $(ZIP_NAME) $(SRCS) $(HEADS) Makefile README.md CHANGELOG.md LICENSE