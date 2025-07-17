include config.mk

LIBENIGMA_SRC=src/lib/enigma.c src/lib/reflectors.c src/lib/rotors.c src/lib/bombe.c

ENIGMA_BIN=enigma
ENIGMA_SRC=src/lib/enigma.c src/lib/reflectors.c src/lib/rotors.c src/enigma_main.c

BOMBE_BIN=bombe
BOMBE_SRC=src/lib/bombe.c src/lib/reflectors.c src/lib/rotors.c src/lib/enigma.c src/bombe_main.c

all: $(ENIGMA_BIN) $(BOMBE_BIN)

$(ENIGMA_BIN): $(ENIGMA_SRC)
	$(CC) $(CFLAGS) $(ENIGMA_CFLAGS) $(CPPFLAGS) $^ -o $@

$(BOMBE_BIN): $(BOMBE_SRC)
	$(LD) $(CFLAGS) $(LDFLAGS) $^ -o $@

clean:
	rm -f $(wildcard src/*.o) $(ENIGMA_BIN) $(BOMBE_BIN)

	mkdir -p enigma-$(VERSION)
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
