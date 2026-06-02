add_executable(test_3dtorib test_3dtorib.cpp ${CMAKE_SOURCE_DIR}/src/3dtorib/3dtorib.cpp)
target_include_directories(test_3dtorib PRIVATE ${CMAKE_SOURCE_DIR}/src/3dtorib)
add_test(NAME test_3dtorib COMMAND test_3dtorib)
