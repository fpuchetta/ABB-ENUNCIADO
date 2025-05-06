CFLAGS = -std=c99 -Wall -Wconversion -Wtype-limits -pedantic -Werror -O2 -g -Isrc
SRC = src/*.c

abb: $(SRC) main.c
	gcc $(CFLAGS) $^ -o $@

clean:
	rm -f abb
