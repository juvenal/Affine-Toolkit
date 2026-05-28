add_executable(closequad closequad.c)
target_link_libraries(closequad ributil m ZLIB::ZLIB)
install(TARGETS closequad RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
