add_executable(mac2unix mac2unix.c)
add_executable(ibm2unix ibm2unix.c)
add_executable(untab untab.c)
install(TARGETS mac2unix ibm2unix untab RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
