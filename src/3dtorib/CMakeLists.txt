add_executable(3dtorib
    main.cpp
    3dtorib.cpp
    3dtorib.hpp
    tiny_obj_loader.h
)

target_include_directories(3dtorib PRIVATE .)
install(TARGETS 3dtorib RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
