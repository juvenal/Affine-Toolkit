# libributil is an aggregate in legacy, but we can just use the libs.
# However, if some code expects libributil.a to exist as a single unit, we might need it.
# Typerib Makefile says it links -lributil.
# Let's create an interface library or a real aggregate if needed.
# For simplicity, let's create a library that includes sconstant.c (needed by typerib).

add_library(ributil STATIC sconstant.c)
target_link_libraries(ributil PUBLIC ribrdr ribhash sribw ZLIB::ZLIB)
target_include_directories(ributil PUBLIC . ${CMAKE_SOURCE_DIR}/include)

add_library(ribnop STATIC ribnop.c)
target_include_directories(ribnop PUBLIC . ${CMAKE_SOURCE_DIR}/include)

add_library(ribtrimstore STATIC trimstore.c)
target_include_directories(ribtrimstore PUBLIC . ${CMAKE_SOURCE_DIR}/include)

add_library(ribattrstore STATIC attrstore.c)
target_include_directories(ribattrstore PUBLIC . ${CMAKE_SOURCE_DIR}/include)

# Executables
add_executable(typerib typerib.c)
target_link_libraries(typerib ributil)

add_executable(ribbish ribbish.c)
target_link_libraries(ribbish ributil ribnop)

add_executable(ribfixer ribfixer.c)
target_link_libraries(ribfixer ributil ribattrstore ribtrimstore)

add_executable(ribobjmod objmod.c)
target_link_libraries(ribobjmod ributil ribnop)

# Tests
add_executable(trimtest trimtest.c)
target_link_libraries(trimtest ribtrimstore ributil)

add_executable(attrtest attrtest.c)
target_link_libraries(attrtest ribattrstore ributil)
