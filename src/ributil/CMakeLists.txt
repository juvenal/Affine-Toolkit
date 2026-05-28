# libributil is an aggregate in legacy, but we can just use the libs.
# However, if some code expects libributil.a to exist as a single unit, we might need it.
# Typerib Makefile says it links -lributil.
# Let's create an interface library or a real aggregate if needed.
# For simplicity, let's create a library that includes sconstant.c (needed by typerib).

add_affine_library(ributil sconstant.c)
affine_target_link_libraries(ributil PUBLIC ribrdr ribhash sribw ZLIB::ZLIB)

add_affine_library(ribnop ribnop.c)

add_affine_library(ribtrimstore trimstore.c)

add_affine_library(ribattrstore attrstore.c)

# Executables
add_executable(typerib typerib.c)
target_link_libraries(typerib ributil)
install(TARGETS typerib RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

add_executable(ribbish ribbish.c)
target_link_libraries(ribbish ributil ribnop)
install(TARGETS ribbish RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

add_executable(ribfixer ribfixer.c)
target_link_libraries(ribfixer ributil ribattrstore ribtrimstore)
install(TARGETS ribfixer RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

add_executable(ribobjmod objmod.c)
target_link_libraries(ribobjmod ributil ribnop)
install(TARGETS ribobjmod RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# Tests
add_executable(trimtest trimtest.c)
target_link_libraries(trimtest ribtrimstore ributil)
install(TARGETS trimtest RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

add_executable(attrtest attrtest.c)
target_link_libraries(attrtest ribattrstore ributil)
install(TARGETS attrtest RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
