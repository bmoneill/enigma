project("libenigma" C)

# Global Parameters
set(PROJECT_NAME "libenigma")
set(LIBRARY_NAME "enigma")
#set(LIBRARY_VERSION_MAJOR dev)

include(GNUInstallDirs)

set(LIBRARY_BASE_PATH ".")

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
 "${LIBRARY_BASE_PATH}/enigma/reflector.c"
 "${LIBRARY_BASE_PATH}/enigma/rotor.c"
 "${LIBRARY_BASE_PATH}/enigma/score.c"
)

set(LIBRARY_PUBLIC_HEADERS
 "${LIBRARY_BASE_PATH}/enigma/common.h"
 "${LIBRARY_BASE_PATH}/enigma/crack.h"
 "${LIBRARY_BASE_PATH}/enigma/enigma.h"
 "${LIBRARY_BASE_PATH}/enigma/io.h"
 "${LIBRARY_BASE_PATH}/enigma/ioc.h"
 "${LIBRARY_BASE_PATH}/enigma/ngram.h"
 "${LIBRARY_BASE_PATH}/enigma/reflector.h"
 "${LIBRARY_BASE_PATH}/enigma/rotor.h"
)

add_library (
 ${LIBRARY_NAME} SHARED ${LIBRARY_PUBLIC_SRC}
)

add_library (
 ${LIBRARY_NAME}_static STATIC ${LIBRARY_PUBLIC_SRC}
)

# Compiler definitions
if(TEST)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DTEST")
    target_compile_options(${LIBRARY_NAME} PRIVATE "-fprofile-arcs" "-ftest-coverage")
    target_link_options(${LIBRARY_NAME} PRIVATE "-fprofile-arcs" "-ftest-coverage")
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -ffast-math")
endif()

# Library installation directives
INSTALL (
 TARGETS ${LIBRARY_NAME}
 LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
 PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
