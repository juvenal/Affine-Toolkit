add_library(sribw STATIC write.c constant.c)
target_include_directories(sribw PUBLIC ${CMAKE_SOURCE_DIR}/include)

install(TARGETS sribw ARCHIVE DESTINATION lib)
