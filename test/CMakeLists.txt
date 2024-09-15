find_package(GTest REQUIRED)

add_executable(olympus_tests
    test_json_mission.cpp
    test_json_team.cpp
    test_json_game.cpp
)
target_link_libraries(olympus_tests PRIVATE
    olympus_library
    GTest::gmock_main
)

gtest_discover_tests(olympus_tests
    DISCOVERY_MODE PRE_TEST
)
