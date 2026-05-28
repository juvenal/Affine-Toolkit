add_library(sribw STATIC
    write.c
    constant.c
)
target_include_directories(sribw PUBLIC . ${CMAKE_SOURCE_DIR}/include)

# testrib
add_executable(testrib
    testrib.c
    test3_7.c
)
target_link_libraries(testrib sribw)
