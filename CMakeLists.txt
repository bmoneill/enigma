# See https://honeytreelabs.com/posts/cmake-unity-integration

cmake_minimum_required(VERSION 3.31.6)
project("libenigma" C)
include(CTest)

# C Standard
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -pedantic -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_XOPEN_SOURCE=700 -D_FILE_OFFSET_BITS=64")

# Warnings
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -Wall -Wextra -Wno-unused-parameter -Wno-unused-function -Wno-unused-variable -Wno-missing-field-initializers")

# Optimization
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -ffast-math")

# Debugging and Profiling
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -pg")

function(Enable_Tests)
  add_subdirectory(external)
  set(TEST ON CACHE INTERNAL "")
endfunction()

function(Build_Library)
  add_subdirectory(src)
  include_directories(src)
endfunction()

function(Build_Tests)
  add_subdirectory(test)
endfunction()

if(TARGET_GROUP STREQUAL test)
  Enable_Tests()
  Build_Library()
  Build_Tests()
elseif(TARGET_GROUP STREQUAL tools)
  Build_Library()
  add_subdirectory(tools)
elseif(TARGET_GROUP STREQUAL lib)
  Build_Library()
else()
  Build_Library()
  add_subdirectory(tools)
endif()
