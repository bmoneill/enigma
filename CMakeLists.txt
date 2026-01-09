# See https://honeytreelabs.com/posts/cmake-unity-integration

cmake_minimum_required(VERSION 3.31.6)
project("libenigma" C)
include(CTest)

option(TEST "Build tests" OFF)
option(LIBONLY "Build libenigma only" OFF)

add_subdirectory(docs/man)

execute_process(
    COMMAND git rev-parse --short HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Version
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DENIGMA_VERSION='\"${GIT_COMMIT_HASH}\"'")

# C Standard
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -pedantic -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_XOPEN_SOURCE=700 -D_FILE_OFFSET_BITS=64")

# Warnings
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -Wall -Wextra -Wno-unused-parameter -Wno-unused-function -Wno-unused-variable -Wno-missing-field-initializers")

function(Enable_Tests)
    if(CMAKE_COMPILER_IS_GNUCC)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pg -fprofile-arcs -ftest-coverage -fsanitize=address,undefined -fno-omit-frame-pointer")
        set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeModules)
        include(CodeCoverage)
    endif()
endfunction()

function(Build_Library)
    add_subdirectory(src)
    include_directories(src)
endfunction()

function(Build_Tests)
    add_subdirectory(external)
    add_subdirectory(test)
endfunction()

if(TEST)
    Enable_Tests()
    Build_Library()
    Build_Tests()
elseif(LIBONLY)
    Build_Library()
else()
    Build_Library()
    add_subdirectory(tools)
endif()
