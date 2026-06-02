macro(add_ribhash_test name)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} ribhash)
    add_test(NAME ${name} COMMAND ${name})
endmacro()

add_ribhash_test(hashtest)
add_ribhash_test(strtest)

add_executable(cptest cptest.cpp)
target_link_libraries(cptest ribcparm)
add_test(NAME cptest COMMAND cptest)

add_executable(cnsttest cnsttest.cpp)
target_link_libraries(cnsttest ribconst)
target_sources(cnsttest PRIVATE ${CMAKE_SOURCE_DIR}/src/ribhash/basistbl.c)
add_test(NAME cnsttest COMMAND cnsttest)
