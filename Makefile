SRC := $(shell find src -name "*.c")
OBJ := $(SRC:.c=.o)

CC  := clang

LFLAGS := -lreadline
CFLAGS := -Wall -Wextra -Werror -pedantic -std=c99 -g -Wno-unused -Wno-unused-parameter -Isrc/

all: $(OBJ)
	$(CC) $(OBJ) $(LFLAGS) -o test

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJ)

check-syntax:
	$(CC) -o nul $(CFLAGS) -S $(CHK_SOURCES)
