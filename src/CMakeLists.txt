project("libenigma" C)

# Global Parameters
set(PROJECT_NAME "libenigma")
set(LIBRARY_NAME "enigma")
set(LIBRARY_VERSION_MAJOR 0)
set(LIBRARY_VERSION_STRING 0.0)

include(GNUInstallDirs)

set(LIBRARY_BASE_PATH ".")

# Math library
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")

# Warnings
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -Wall -Wextra -Wno-unused-parameter -Wno-unused-function -Wno-unused-variable")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-missing-field-initializers -Wmissing-prototypes -Wshadow")

# Optimization
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -ffast-math")

# Debugging and Profiling
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -pg -fsanitize=address,undefined -fno-omit-frame-pointer")

INCLUDE_DIRECTORIES (
	"${LIBRARY_BASE_PATH}"
)

set(LIBRARY_PUBLIC_SRC
	"${LIBRARY_BASE_PATH}/enigma/crack.c"
	"${LIBRARY_BASE_PATH}/enigma/enigma.c"
	"${LIBRARY_BASE_PATH}/enigma/io.c"
	"${LIBRARY_BASE_PATH}/enigma/ioc.c"
	"${LIBRARY_BASE_PATH}/enigma/ngram.c"
	"${LIBRARY_BASE_PATH}/enigma/ngram.c"
)

set(LIBRARY_PUBLIC_HEADERS
	"${LIBRARY_BASE_PATH}/enigma/common.h"
	"${LIBRARY_BASE_PATH}/enigma/crack.h"
	"${LIBRARY_BASE_PATH}/enigma/enigma.h"
	"${LIBRARY_BASE_PATH}/enigma/io.h"
	"${LIBRARY_BASE_PATH}/enigma/ioc.h"
	"${LIBRARY_BASE_PATH}/enigma/ngram.h"
	"${LIBRARY_BASE_PATH}/enigma/reflectors.h"
	"${LIBRARY_BASE_PATH}/enigma/rotors.h"
)

add_library (
	${LIBRARY_NAME}_static STATIC ${LIBRARY_PUBLIC_SRC}
)

if(NOT ${TARGET_GROUP} STREQUAL wasm)
    add_library (
        ${LIBRARY_NAME} SHARED ${LIBRARY_PUBLIC_SRC} ${LIBRARY_PUBLIC_HEADERS}
	)
endif()

# Compiler definitions
if(TEST)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DTEST")
endif()

# Library installation directives
INSTALL (
	TARGETS ${BINARY_NAME}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
