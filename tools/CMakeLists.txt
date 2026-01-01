set(ENIGMA_BINARY_NAME "enigmacli")
set(CRACK_BINARY_NAME "enigmacrack")
set(GENNGRAMS_SCRIPT_NAME "genngrams.sh")
set(GENNGRAMS_SCRIPT_TARGET_NAME "genngrams")

add_executable(${ENIGMA_BINARY_NAME} enigma_main.c)
add_executable(${CRACK_BINARY_NAME} crack_main.c)

# Set the version for the executables
target_compile_definitions(${ENIGMA_BINARY_NAME} PRIVATE VERSION="${GIT_COMMIT_HASH}")
target_compile_definitions(${CRACK_BINARY_NAME} PRIVATE VERSION="${GIT_COMMIT_HASH}")

# Include the source directory for the executables
include_directories(${PROJECT_SOURCE_DIR}/src)

target_link_libraries(${ENIGMA_BINARY_NAME} enigma_static m)
target_link_libraries(${CRACK_BINARY_NAME} enigma_static m)

install(TARGETS ${ENIGMA_BINARY_NAME} ${CRACK_BINARY_NAME}
  RUNTIME DESTINATION bin)
install(PROGRAMS ${GENNGRAMS_SCRIPT_NAME} DESTINATION bin RENAME ${GENNGRAMS_SCRIPT_TARGET_NAME})
