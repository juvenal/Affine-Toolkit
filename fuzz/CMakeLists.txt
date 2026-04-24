# libFuzzer targets — only built when ENABLE_FUZZING=ON (requires Clang)
add_executable(fuzz_rib_parser fuzz_rib_parser.c)
target_link_libraries(fuzz_rib_parser PRIVATE ribrdr)
target_compile_options(fuzz_rib_parser PRIVATE -fsanitize=fuzzer,address)
target_link_options(fuzz_rib_parser PRIVATE -fsanitize=fuzzer,address)

# Seed corpus — use existing RIB test files as starting input
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/corpus)
file(GLOB RIB_SEEDS
    ${CMAKE_SOURCE_DIR}/readrib/tests/*.rib
    ${CMAKE_SOURCE_DIR}/readrib/tests/*.asc
    ${CMAKE_SOURCE_DIR}/examples/closequad/*.rib)
foreach(seed ${RIB_SEEDS})
    get_filename_component(seedname ${seed} NAME)
    configure_file(${seed} ${CMAKE_CURRENT_BINARY_DIR}/corpus/${seedname} COPYONLY)
endforeach()
