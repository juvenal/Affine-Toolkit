add_executable(ribtree ribtree.c)
target_link_libraries(ribtree PRIVATE sribw)
install(TARGETS ribtree RUNTIME DESTINATION bin)
