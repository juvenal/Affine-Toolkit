# ── New unit tests (Phase 4b) ─────────────────────────────────────────────────

# test_string_safety: pure-C, no library deps
add_executable(test_string_safety test_string_safety.c)
add_test(NAME test_string_safety COMMAND test_string_safety)

# test_hash_api: exercises ribhash public API including const-correct functions
add_executable(test_hash_api test_hash_api.c)
target_link_libraries(test_hash_api PRIVATE ribhash)
add_test(NAME test_hash_api COMMAND test_hash_api)

# test_rib_reader: integration tests for RibOpen / RibRead / RibGetRIFromString
add_executable(test_rib_reader test_rib_reader.c)
target_link_libraries(test_rib_reader PRIVATE ribrdr)
add_test(NAME test_rib_reader COMMAND test_rib_reader)
