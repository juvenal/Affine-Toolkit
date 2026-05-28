set(RDROBJS
    readrib.c
    streamio.c
    paraml.c
    seterror.c
    vcache.c
    readfunc.c
    hintfunc.c
    ribfunc.c
    declare.c
    ribtbl.c
    hinttbl.c
)

set(TABLEOBJS ritable.c)
set(ERROROBJS error.c)

# libribrdr
add_affine_library(ribrdr 
    ${RDROBJS} 
    ${TABLEOBJS} 
    ${ERROROBJS}
)
affine_target_link_libraries(ribrdr PUBLIC ribhash sribw ZLIB::ZLIB)

# libribtable
add_affine_library(ribtable ${TABLEOBJS})

# libriberror
add_affine_library(riberror ${ERROROBJS})

# libsribrdr
add_affine_library(sribrdr ${RDROBJS})
affine_target_link_libraries(sribrdr PUBLIC ribhash sribw ZLIB::ZLIB)

# Tests
add_executable(strmtest strmtest.c)
target_link_libraries(strmtest ribrdr ribhash ZLIB::ZLIB)
install(TARGETS strmtest RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

add_executable(readtest readtest.c)
target_link_libraries(readtest ribrdr sribhash sribw ZLIB::ZLIB)
install(TARGETS readtest RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

add_executable(hinttest hinttest.c)
target_link_libraries(hinttest ribrdr sribhash sribw ZLIB::ZLIB)
install(TARGETS hinttest RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

add_executable(errtest errtest.c)
target_link_libraries(errtest riberror ribmsg)
install(TARGETS errtest RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
