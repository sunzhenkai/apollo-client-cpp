find_package(GTest REQUIRED)
enable_testing()

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/gtests TEST_SOURCES)
add_executable(gtest_main ${TEST_SOURCES})
target_link_libraries(gtest_main ${THIRD_LIBRARIES} GTest::GTest GTest::Main)
