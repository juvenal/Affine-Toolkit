macro(add_readrib_test name)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} ribrdr sribhash sribw ZLIB::ZLIB)
    add_test(NAME ${name} COMMAND ${name})
    set_tests_properties(${name} PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endmacro()

add_readrib_test(strmtest)
add_readrib_test(readtest)
add_readrib_test(hinttest)

add_executable(errtest errtest.cpp)
target_link_libraries(errtest ribrdr ribhash riberror ribmsg)
add_test(NAME errtest COMMAND errtest)
