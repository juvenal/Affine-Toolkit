add_executable(hello_rib hello_rib.c)
target_link_libraries(hello_rib PRIVATE ribrdr)
install(TARGETS hello_rib RUNTIME DESTINATION bin)
