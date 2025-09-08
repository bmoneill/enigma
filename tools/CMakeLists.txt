set(ENIGMA_BINARY_NAME "enigmacli")
set(BOMBE_BINARY_NAME "bombe")
set(CRACK_BINARY_NAME "enigmacrack")

# Get git commit hash
execute_process(
    COMMAND git rev-parse --short HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

add_executable(${ENIGMA_BINARY_NAME} enigma_main.c)
add_executable(${BOMBE_BINARY_NAME} bombe_main.c)
add_executable(${CRACK_BINARY_NAME} crack_main.c)

# Set the version for the executables
target_compile_definitions(${ENIGMA_BINARY_NAME} PRIVATE VERSION="${GIT_COMMIT_HASH}")
target_compile_definitions(${BOMBE_BINARY_NAME} PRIVATE VERSION="${GIT_COMMIT_HASH}")
target_compile_definitions(${CRACK_BINARY_NAME} PRIVATE VERSION="${GIT_COMMIT_HASH}")

# Include the source directory for the executables
include_directories(${PROJECT_SOURCE_DIR}/src)

target_link_libraries(${ENIGMA_BINARY_NAME} enigma)
target_link_libraries(${BOMBE_BINARY_NAME} enigma)
target_link_libraries(${CRACK_BINARY_NAME} enigma)
