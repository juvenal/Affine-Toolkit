add_executable(3dtorib
    main.cpp
    3dtorib.cpp
    3dtorib.hpp
    tiny_obj_loader.h
)

target_include_directories(3dtorib PRIVATE .)
install(TARGETS 3dtorib RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# Add tests
enable_testing()
add_executable(test_3dtorib
    tests/test_3dtorib.cpp
    3dtorib.cpp
)
target_include_directories(test_3dtorib PRIVATE .)
add_test(NAME test_3dtorib COMMAND test_3dtorib)
