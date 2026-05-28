set(RIBHASH_SOURCES
    hash.c
    constant.c
    basistbl.c
    string.c
    errormsg.c
    cparams.c
    names.c
    tables.c
)

add_library(ribhash STATIC ${RIBHASH_SOURCES})
target_include_directories(ribhash PUBLIC . ${CMAKE_CURRENT_SOURCE_DIR}/../include)

# SRIBHASH (subset)
set(SRIBHASH_SOURCES
    hash.c
    basistbl.c
    string.c
    errormsg.c
    cparams.c
    names.c
    tables.c
)
add_library(sribhash STATIC ${SRIBHASH_SOURCES})
target_include_directories(sribhash PUBLIC . ${CMAKE_CURRENT_SOURCE_DIR}/../include)

# Individual libs
add_library(ribconst STATIC constant.c)
add_library(ribtables STATIC tables.c)
add_library(ribmsg STATIC errormsg.c)
add_library(ribcparm STATIC cparams.c names.c)

# Binary
add_executable(isaprime isaprime.c)
target_link_libraries(isaprime m) # Link math library

# Tests
add_executable(hashtest hashtest.c)
target_link_libraries(hashtest ribhash)

add_executable(strtest strtest.c)
target_link_libraries(strtest ribhash)

add_executable(cptest cptest.c)
target_link_libraries(cptest ribcparm)

add_executable(cnsttest cnsttest.c)
target_link_libraries(cnsttest ribconst)
# Need basistbl.o too according to Makefile
target_sources(cnsttest PRIVATE basistbl.c)
