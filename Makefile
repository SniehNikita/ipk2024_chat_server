# @file makefile
# @author Sniehovskyi Nikita (xsnieh00)
# @date 11.04.2024
# @brief Builds project. Use 'make' to build

CC = gcc
CFLAGS = -Wall -Wextra --std=c11

EXEC = ipk24chat-server

ZIP_NAME = "xsnieh00.zip"

SRCDIR = src/
OBJDIR = objs/

# HEADS = $(wildcard $(SRCDIR)*.h)
HEADS = src/main.h
# SRCS = $(wildcard $(SRCDIR)*.c)
SRCS = src/main.c
# OBJS = $(patsubst $(SRCDIR)%.c,$(OBJDIR)%.o,$(SRCS))
OBJS = objs/main.o

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