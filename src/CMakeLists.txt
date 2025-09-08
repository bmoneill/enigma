project("libenigma" C)

# Global Parameters
set(PROJECT_NAME "libenigma")
set(LIBRARY_NAME "enigma")
set(LIBRARY_VERSION_MAJOR 0)
set(LIBRARY_VERSION_STRING 0.0)

include(GNUInstallDirs)

set(LIBRARY_BASE_PATH ".")

INCLUDE_DIRECTORIES (
	"${LIBRARY_BASE_PATH}"
)

set(LIBRARY_PUBLIC_SRC
	"${LIBRARY_BASE_PATH}/enigma/bombe.c"
	"${LIBRARY_BASE_PATH}/enigma/common.c"
	"${LIBRARY_BASE_PATH}/enigma/crack.c"
	"${LIBRARY_BASE_PATH}/enigma/enigma.c"
	"${LIBRARY_BASE_PATH}/enigma/ioc.c"
	"${LIBRARY_BASE_PATH}/enigma/ngram.c"
)

set(LIBRARY_PUBLIC_HEADERS
	"${LIBRARY_BASE_PATH}/bombe.h"
	"${LIBRARY_BASE_PATH}/common.h"
	"${LIBRARY_BASE_PATH}/crack.h"
	"${LIBRARY_BASE_PATH}/enigma.h"
	"${LIBRARY_BASE_PATH}/ioc.h"
	"${LIBRARY_BASE_PATH}/ngram.h"
	"${LIBRARY_BASE_PATH}/reflectors.h"
	"${LIBRARY_BASE_PATH}/rotors.h"
)

add_library (
	${LIBRARY_NAME} STATIC ${LIBRARY_PUBLIC_SRC}
)

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
