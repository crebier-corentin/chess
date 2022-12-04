add_executable(chess_test main.c)
target_link_libraries(chess_test PRIVATE libchess)
add_test(NAME chess_test COMMAND chess_test)
