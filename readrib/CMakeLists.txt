set(READRIB_SOURCES
    readrib.c
    streamio.c
    paraml.c
    seterror.c
    vcache.c
    readfunc.c
    hintfunc.c
    ribfunc.c
    declare.c
    ribtbl.c
    hinttbl.c
    ritable.c
    error.c
    version.c)

# Full reader library — includes hash + writer constants
add_library(ribrdr STATIC ${READRIB_SOURCES})
target_include_directories(ribrdr PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(ribrdr PUBLIC ribhash sribw ZLIB::ZLIB)

# Stripped reader — without constant.c and errormsg.c (consumers supply their own)
add_library(sribrdr STATIC ${READRIB_SOURCES})
target_include_directories(sribrdr PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(sribrdr PUBLIC sribhash sribw ZLIB::ZLIB)

# Test executables
foreach(test strmtest errtest readtest hinttest)
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${test}.c)
        add_executable(${test} ${test}.c)
        target_link_libraries(${test} PRIVATE ribrdr)
    endif()
endforeach()

install(TARGETS ribrdr ARCHIVE DESTINATION lib)
