CC := gcc
OUT := ./cfetch
CFLAGS := -O3 -Wall -std=gnu11 -lcpuid -lX11
SRC := *.c

all: cfetch

.PHONY: cfetch clean install

cfetch:
	$(CC) $(CFLAGS) -o $(OUT) $(SRC)

debug:
	$(CC) -DDEBUG $(CFLAGS) -g -ggdb -o $(OUT).debug $(SRC)

install: cfetch
	install ./cfetch /usr/bin

clean:
	rm $(OUT)* || :


