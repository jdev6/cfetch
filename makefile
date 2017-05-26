CC := gcc
OUT := ./cfetch
CFLAGS := -O3 -Wall -std=gnu11 -lcpuid
SRC := *.c

all: cfetch

.PHONY: cfetch clean install

cfetch:
	$(CC) $(CFLAGS) -o $(OUT) $(SRC)

install: cfetch
	install ./cfetch /usr/bin

clean:
	rm $(OUT) || :


