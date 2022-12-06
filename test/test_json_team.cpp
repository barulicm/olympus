#include "json_team.hpp"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

TEST(JsonTeam, LoadTeam)
{
    nlohmann::json data = nlohmann::json::parse(R"({
    "number": "1234",
    "name": "My Test Team",
    "scores": [0, 100, 200]
})");
    const auto team = data.get<Team>();
    EXPECT_EQ(team.number, "1234");
    EXPECT_EQ(team.name, "My Test Team");
    const std::vector<int> expected_scores = {0,100,200};
    EXPECT_THAT(team.scores, testing::ContainerEq(expected_scores));
}

TEST(JsonTeam, DumpTeam)
{
    const Team team{
        .number = "8765",
        .name = "Another Team",
        .scores = {400, 0, 150}
    };
    const nlohmann::json data = team;
    const nlohmann::json expected_data = {
            {"number", "8765"},
            {"name", "Another Team"},
            {"scores", {400,0,150}}
    };
    EXPECT_EQ(data, expected_data);
}
