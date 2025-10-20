add_executable(enigma_tests test_enigma.c)
target_link_libraries(enigma_tests enigma Unity)
add_test(enigma enigma_tests)

add_executable(io_tests test_io.c)
target_link_libraries(io_tests enigma Unity)
add_test(io io_tests)
