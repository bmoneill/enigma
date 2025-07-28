include config.mk

LIBENIGMA_SRC = src/bombe.c src/enigma.c src/reflectors.c src/rotors.c
LIBENIGMA_OBJ = src/bombe.o src/enigma.o src/reflectors.o src/rotors.o
LIBENIGMA =     libenigma.a

ENIGMA_BIN = enigma
ENIGMA_SRC = tools/enigma_main.c

BOMBE_BIN = bombe
BOMBE_SRC = tools/bombe_main.c

CRACK_BIN = crack
CRACK_SRC = src/enigma.c src/reflectors.c src/rotors.c src/ioc.c src/ngram.c src/crack.c src/crack_main.c

all: $(ENIGMA_BIN) $(BOMBE_BIN) #$(CRACK_BIN)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

$(LIBENIGMA): $(LIBENIGMA_OBJ)
	ar rcs $@ $^

$(ENIGMA_BIN): $(ENIGMA_SRC) $(LIBENIGMA)
	$(LD) $(CFLAGS) $(TOOLS_CFLAGS) $(LDFLAGS) $^ -o $@

$(BOMBE_BIN): $(BOMBE_SRC) $(LIBENIGMA)
	$(LD) $(CFLAGS) $(TOOLS_CFLAGS) $(LDFLAGS) $^ -o $@

$(CRACK_BIN): $(CRACK_SRC) $(LIBENIGMA)
	$(LD) $(CFLAGS) $(TOOLS_CFLAGS) $(LDFLAGS) $^ -o $@

clean:
	rm -f $(ENIGMA_BIN) $(BOMBE_BIN) $(CRACK_BIN) $(LIBENIGMA) src/*.o tools/*.o

dist:
	mkdir -p enigma-$(VERSION)
	cp -rf src/ README.md LICENSE config.mk Makefile enigma-$(VERSION)
	tar -cf enigma-$(VERSION).tar enigma-$(VERSION)
	gzip enigma-$(VERSION).tar
	rm -rf enigma-$(VERSION)

install: $(ENIGMA_BIN) $(BOMBE_BIN)
	mkdir -p $(DESTDIR)/$(PREFIX)/bin
	cp -f $(ENIGMA_BIN) $(DESTDIR)$(PREFIX)/bin/enigma
	cp -f $(BOMBE_BIN) $(DESTDIR)$(PREFIX)/bin/bombe
	chmod 755 $(DESTDIR)$(PREFIX)/bin/enigma
	chmod 755 $(DESTDIR)$(PREFIX)/bin/bombe

uninstall:
	rm -f $(DESTDIR)/$(PREFIX)/bin/enigma
	rm -f $(DESTDIR)/$(PREFIX)/bin/bombe

.PHONY: all $(ENIGMA_BIN) $(BOMBE_BIN) clean dist install uninstall
