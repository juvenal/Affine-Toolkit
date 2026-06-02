add_executable(trimtest trimtest.cpp)
target_link_libraries(trimtest ribtrimstore ributil)
add_test(NAME trimtest COMMAND trimtest)

add_executable(attrtest attrtest.cpp)
target_link_libraries(attrtest ribattrstore ributil)
add_test(NAME attrtest COMMAND attrtest)
