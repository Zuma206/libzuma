cc ?= cc
ccf = $(cc) $(cflags) -Wall -Wextra -Werror -std=c23

libzuma.o: zuma.c zuma.h
	$(ccf) -o $@ -c zuma.c

test: test.c libzuma.o
	$(ccf) -o $@ $^
