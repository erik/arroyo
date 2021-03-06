SRC := $(shell find src -name "*.c")
OBJ := $(SRC:.c=.o)
DEPS := $(SRC:.c=.d)

CC  := gcc

DFLAGS := -ggdb -O0
LFLAGS := -lreadline -lm
CFLAGS := -Wall -Wextra -pedantic -std=c99 -Wno-unused -Wno-unused-parameter -Iinclude/

.PHONY= all debug profile clean test todo loc sloc check-syntax

all: $(OBJ)
	$(CC) $(OBJ) $(LFLAGS) -o arroyo

-include $(SRC:.c=.d)

%.o: %.c
	$(CC) -c -MMD $(CFLAGS) $< -o $@

debug:
	$(MAKE) all "CFLAGS=$(CFLAGS) $(DFLAGS)"

profile:
	$(MAKE) debug "CFLAGS=$(CFLAGS) -pg"
	$(CC) $(OBJ) $(LFLAGS) -pg -o arroyo

clean:
	rm -f $(OBJ) $(DEPS)

test: debug
	valgrind --show-reachable=yes --leak-check=full --track-origins=yes ./arroyo test/runner.arr

todo:
	@ack 'XXX|TODO|FIXME'
	@cat TODO

loc:
	@ack --type=cc -f | xargs wc -l | sort -h

sloc:
	@cloc src include

check-syntax:
	$(CC) -o nul $(CFLAGS) -S $(CHK_SOURCES)
