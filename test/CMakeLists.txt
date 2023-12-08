find_package(GTest REQUIRED)
set(CMAKE_GTEST_DISCOVER_TESTS_DISCOVERY_MODE PRE_TEST)

add_executable(olympus_tests
        test_json_mission.cpp
        test_json_team.cpp)
target_link_libraries(olympus_tests PRIVATE
  olympus_library
  GTest::gtest
  GTest::gtest_main
  GTest::gmock
)

gtest_discover_tests(olympus_tests)
