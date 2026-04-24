add_library(ribhash STATIC
    hash.c
    constant.c
    basistbl.c
    string.c
    errormsg.c
    cparams.c
    names.c
    tables.c)

target_include_directories(ribhash PUBLIC ${CMAKE_SOURCE_DIR}/include)

# Stripped variant — omits constant.c (used where embedded RIB constants aren't needed)
add_library(sribhash STATIC
    hash.c
    basistbl.c
    string.c
    errormsg.c
    cparams.c
    names.c
    tables.c)
target_include_directories(sribhash PUBLIC ${CMAKE_SOURCE_DIR}/include)

# Test executables
add_executable(hashtest hashtest.c)
target_link_libraries(hashtest PRIVATE ribhash)

add_executable(strtest strtest.c)
target_link_libraries(strtest PRIVATE ribhash)

add_executable(cnsttest cnsttest.c)
target_link_libraries(cnsttest PRIVATE ribhash)

add_executable(cptest cptest.c)
target_link_libraries(cptest PRIVATE ribhash)

add_executable(isaprime isaprime.c)
target_link_libraries(isaprime PRIVATE m)

install(TARGETS ribhash ARCHIVE DESTINATION lib)
