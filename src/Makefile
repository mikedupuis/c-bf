PREFIX = /usr/local
BINDIR = /usr/local/bin
SRC = bf.c
CFLAGS = -std=c99 -Wall -pedantic
BIN = bf

.PHONY: clean uninstall

$(BIN):
	$(CC) $(SRC) $(CFLGAGS) -o $@

install: $(BIN)
	install $(BIN) $(BINDIR)

clean:
	rm -f $(BIN)

uninstall:
	rm -f $(BINDIR)/$(BIN)
