VERSION       = 0.1

PREFIX        = /usr/local

CC            = gcc
LD            = $(CC)

CFLAGS        = -Werror -std=c99 -pedantic -Ofast
# CFLAGS      += -g -pg
# CFLAGS      += -DVERBOSE
CPPFLAGS      = -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_XOPEN_SOURCE=700 \
                -D_FILE_OFFSET_BITS=64 -DVERSION="$(VERSION)"
