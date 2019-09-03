.DEFAULT_GOAL := all
CFLAGS=-I. -Werror -Wall -Wextra -ftrapv -g -pedantic -fsanitize=address -fsanitize=undefined -Wsign-conversion -pedantic-errors -fsanitize-undefined-trap-on-error

ODIR=obj

$(ODIR)/fk_circular_buffer.o: fk_circular_buffer.c fk_circular_buffer.h
	mkdir -p $(ODIR)
	$(CC) -c -o $@ $< $(CFLAGS) $(LIBS)

fuzz/fuzz_driver: $(ODIR)/fk_circular_buffer.o fuzz/fuzz_driver.c
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

test/test_runner: $(ODIR)/fk_circular_buffer.o test/test_circular_buffer.c
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: test

test: test/test_runner
	test/test_runner

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o fuzz/fuzz_driver test/test_runner *~ core

all: fuzz/fuzz_driver test/test_runner