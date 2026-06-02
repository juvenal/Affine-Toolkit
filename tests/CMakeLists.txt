enable_testing()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src/include)

add_subdirectory(ribhash)
add_subdirectory(readrib)
add_subdirectory(ributil)
add_subdirectory(3dtorib)
