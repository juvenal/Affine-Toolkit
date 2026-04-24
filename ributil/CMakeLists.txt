# ── Core utility libraries ─────────────────────────────────────────────────────

add_library(ribattrstore STATIC attrstore.c)
target_include_directories(ribattrstore PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(ribattrstore PUBLIC ribrdr)

add_library(ribtrimstore STATIC trimstore.c)
target_include_directories(ribtrimstore PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(ribtrimstore PUBLIC ribrdr)

add_library(ribnop STATIC ribnop.c)
target_include_directories(ribnop PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(ribnop PUBLIC ribrdr)

# libributil — aggregate library bundling all readrib + ribhash + sribw objects
# plus the utility-layer sources
add_library(ributil STATIC
    attrstore.c
    trimstore.c
    ribnop.c
    ribfunc.c
    ritable.c
    ribcpnames.c
    sconstant.c
    objmod.c)
target_include_directories(ributil
    PUBLIC  ${CMAKE_SOURCE_DIR}/include
    PRIVATE ${CMAKE_SOURCE_DIR}/readrib)
target_link_libraries(ributil PUBLIC ribrdr ZLIB::ZLIB)

# ── Executables ────────────────────────────────────────────────────────────────

add_executable(typerib typerib.c)
target_link_libraries(typerib PRIVATE ributil m)

add_executable(ribbish ribbish.c)
target_link_libraries(ribbish PRIVATE ributil ribattrstore ribtrimstore)

add_executable(ribfixer ribfixer.c)
target_link_libraries(ribfixer PRIVATE ributil ribattrstore ribtrimstore)

add_executable(ribobjmod objmod.c)
target_link_libraries(ribobjmod PRIVATE ributil)

install(TARGETS ributil ribattrstore ribtrimstore ribnop ARCHIVE DESTINATION lib)
install(TARGETS typerib ribbish ribfixer RUNTIME DESTINATION bin)

# ── Test executables ───────────────────────────────────────────────────────────

add_executable(attrtest attrtest.c)
target_link_libraries(attrtest PRIVATE ributil ribattrstore)

add_executable(trimtest trimtest.c)
target_link_libraries(trimtest PRIVATE ributil ribtrimstore)

add_test(NAME ributil_attrtest COMMAND attrtest)
set_tests_properties(ributil_attrtest PROPERTIES
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME ributil_trimtest COMMAND trimtest)
set_tests_properties(ributil_trimtest PROPERTIES
    FAIL_REGULAR_EXPRESSION "[Ee]rror|FAIL"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
