add_affine_library(sribw
    write.c
    constant.c
)

# testrib
add_executable(testrib
    testrib.c
    test3_7.c
)
target_link_libraries(testrib sribw)
install(TARGETS testrib RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
