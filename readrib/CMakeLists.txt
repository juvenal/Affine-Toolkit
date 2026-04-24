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

install(TARGETS ribrdr sribrdr ARCHIVE DESTINATION lib)

# CTest — all tests need WORKING_DIRECTORY to resolve ./tests/*.asc paths
# strmtest always exits 0; filter its output for obvious failure strings
if(TARGET strmtest)
    add_test(NAME readrib_strmtest COMMAND strmtest)
    set_tests_properties(readrib_strmtest PROPERTIES
        FAIL_REGULAR_EXPRESSION "^Error|FAIL"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif()

# errtest intentionally prints ERROR: messages to exercise the error handler;
# those are expected output, not failures — rely on exit code only
if(TARGET errtest)
    add_test(NAME readrib_errtest COMMAND errtest)
    set_tests_properties(readrib_errtest PROPERTIES
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif()

foreach(t readtest hinttest)
    if(TARGET ${t})
        add_test(NAME readrib_${t} COMMAND ${t})
        set_tests_properties(readrib_${t} PROPERTIES
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    endif()
endforeach()
