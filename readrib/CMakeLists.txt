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
add_library(ribrdr STATIC 
    ${RDROBJS} 
    ${TABLEOBJS} 
    ${ERROROBJS}
)
target_link_libraries(ribrdr PUBLIC ribhash sribw ZLIB::ZLIB)
target_include_directories(ribrdr PUBLIC . ${CMAKE_SOURCE_DIR}/include)

# libsribrdr
add_library(sribrdr STATIC ${RDROBJS})
target_include_directories(sribrdr PUBLIC . ${CMAKE_SOURCE_DIR}/include)

# libribtable
add_library(ribtable STATIC ${TABLEOBJS})
target_include_directories(ribtable PUBLIC . ${CMAKE_SOURCE_DIR}/include)

# libriberror
add_library(riberror STATIC ${ERROROBJS})
target_include_directories(riberror PUBLIC . ${CMAKE_SOURCE_DIR}/include)

# Tests
add_executable(strmtest strmtest.c)
target_link_libraries(strmtest ribrdr ribhash ZLIB::ZLIB)

add_executable(readtest readtest.c)
target_link_libraries(readtest ribrdr sribhash sribw ZLIB::ZLIB)

add_executable(hinttest hinttest.c)
target_link_libraries(hinttest ribrdr sribhash sribw ZLIB::ZLIB)

add_executable(errtest errtest.c)
target_link_libraries(errtest riberror ribmsg)
