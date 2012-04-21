SRC := $(shell find src -name "*.c")
OBJ := $(SRC:.c=.o)

CC  := gcc

DFLAGS := -ggdb -O0
LFLAGS := -lreadline -lm
CFLAGS := -Wall -Wextra -pedantic -std=c99 -Wno-unused -Wno-unused-parameter -Iinclude/

all: $(OBJ)
	$(CC) $(OBJ) $(LFLAGS) -o arroyo

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

debug:
	$(MAKE) all "CFLAGS=$(CFLAGS) $(DFLAGS)"

profile:
	$(MAKE) debug "CFLAGS=$(CFLAGS) -pg"
	$(CC) $(OBJ) $(LFLAGS) -pg -o arroyo

clean:
	rm -f $(OBJ)

todo:
	@ack 'XXX|TODO|FIXME'
	@cat TODO

loc:
	@ack --type=cc -f | xargs wc -l | sort -h

sloc:
	@cloc src include

check-syntax:
	$(CC) -o nul $(CFLAGS) -S $(CHK_SOURCES)
