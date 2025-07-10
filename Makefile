include config.mk

BIN=enigma

SRC=src/enigma.c src/main.c

all: $(BIN)

$(BIN): $(SRC)
	$(LD) $(LDFLAGS) $^ -o $@

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $^

clean:
	rm -f $(wildcard src/*.o) $(BIN)

dist:
	mkdir -p enigma-$(VERSION)
	cp -rf src/enigma.c src/main.c README.md LICENSE config.mk Makefile enigma-$(VERSION)
	tar -cf enigma-$(VERSION).tar enigma-$(VERSION)
	gzip enigma-$(VERSION).tar
	rm -rf enigma-$(VERSION)

install:
	mkdir -p $(DESTDIR)/$(PREFIX)/bin
	cp -f enigma $(DESTDIR)$(PREFIX)/bin/enigma
	chmod 755 $(DESTDIR)$(PREFIX)/bin/enigma

uninstall:
	rm -f $(DESTDIR)/$(PREFIX)/bin/enigma

.PHONY: all clean dist install uninstall
