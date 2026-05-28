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

add_affine_library(ribhash ${RIBHASH_SOURCES})

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
add_affine_library(sribhash ${SRIBHASH_SOURCES})

# Individual libs
add_affine_library(ribconst constant.c)

# Link sribhash against ribconst to resolve symbols for shared library
affine_target_link_libraries(sribhash PUBLIC ribconst)

add_affine_library(ribtables tables.c)
add_affine_library(ribmsg errormsg.c)
add_affine_library(ribcparm cparams.c names.c)

# Binary
add_executable(isaprime isaprime.c)
target_link_libraries(isaprime m) # Link math library
install(TARGETS isaprime RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

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
