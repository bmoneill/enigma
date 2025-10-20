add_executable(enigma_tests test_enigma.c)
target_link_libraries(enigma_tests enigma Unity)
add_test(enigma enigma_tests)
