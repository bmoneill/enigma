VERSION  = 0.2

PREFIX   = /usr/local

CC       = cc
LD       = $(CC)

CFLAGS        = -Werror -std=c99 -pedantic #-g -pg
#ENIGMA_CFLAGS = -DVERBOSE
CPPFLAGS = -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_XOPEN_SOURCE=700 \
           -D_FILE_OFFSET_BITS=64 -DVERSION="$(VERSION)"
