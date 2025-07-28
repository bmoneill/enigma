VERSION       = 0.1

PREFIX        = /usr/local

CC            = gcc
LD            = $(CC)

CFLAGS        = -Werror -Wall -Wextra -Wno-unused-parameter -std=c99 -pedantic
CFLAGS        += -Ofast # For GCC optimizations
#CFLAGS      += -O3 -ffast-math # For Clang optimizations
# CFLAGS      += -g -pg # For debugging and profiling
# CFLAGS      += -DVERBOSE # For verbose output

CPPFLAGS      = -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_XOPEN_SOURCE=700 \
                -D_FILE_OFFSET_BITS=64 -DVERSION="$(VERSION)"
