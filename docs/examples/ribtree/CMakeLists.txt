add_executable(ribtree ribtree.c)
target_link_libraries(ribtree ributil m ZLIB::ZLIB)
install(TARGETS ribtree RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
