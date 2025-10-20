add_executable(crack_tests test_crack.c)
target_link_libraries(crack_tests enigma Unity m)
add_test(crack crack_tests)

add_executable(enigma_tests test_enigma.c)
target_link_libraries(enigma_tests enigma Unity m)
add_test(enigma enigma_tests)

add_executable(io_tests test_io.c)
target_link_libraries(io_tests enigma Unity m)
add_test(io io_tests)

add_executable(ioc_tests test_ioc.c)
target_link_libraries(ioc_tests enigma Unity m)
add_test(ioc ioc_tests)

add_executable(ngram_tests test_ngram.c)
target_link_libraries(ngram_tests enigma Unity m)
add_test(ngram ngram_tests)
