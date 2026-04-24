add_executable(closequad closequad.c)
target_link_libraries(closequad PRIVATE ributil m ZLIB::ZLIB)
install(TARGETS closequad RUNTIME DESTINATION bin)
