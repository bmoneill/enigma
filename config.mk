VERSION       = 0.1

PREFIX        = /usr/local

CC            = gcc
LD            = $(CC)

CFLAGS        = -Werror -Wall -Wextra -Wno-unused-parameter -std=c99 -pedantic
CFLAGS        += -O3 -ffast-math # For optimizations
# CFLAGS      += -g -pg # For debugging and profiling
# CFLAGS      += -DVERBOSE # For verbose output

TOOLS_CFLAGS  = -I./src -I./tools

CPPFLAGS      = -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_XOPEN_SOURCE=700 \
                -D_FILE_OFFSET_BITS=64 -DVERSION="$(VERSION)"
