include config.mk

ENIGMA_BIN=enigma
ENIGMA_SRC=src/enigma.c src/reflectors.c src/rotors.c src/enigma_main.c

BOMBE_BIN=bombe
BOMBE_SRC=src/bombe.c src/reflectors.c src/rotors.c src/enigma.c src/bombe_main.c

all: $(ENIGMA_BIN) $(BOMBE_BIN)

$(ENIGMA_BIN): $(ENIGMA_SRC)
	$(CC) $(CFLAGS) $(CPPFLAGS) $^ -o $@

$(BOMBE_BIN): $(BOMBE_SRC)
	$(LD) $(CFLAGS) $(LDFLAGS) $^ -o $@

clean:
	rm -f $(wildcard src/*.o) $(ENIGMA_BIN) $(BOMBE_BIN)

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
