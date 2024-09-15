#include "state_description/json_team.hpp"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

TEST(JsonTeam, LoadTeam)
{
    nlohmann::json data = nlohmann::json::parse(R"({
    "number": "1234",
    "name": "My Test Team",
    "scores": [0, 100, 200],
    "gpScores": [4, 3, 2],
    "displayScore": 200,
    "rank": 0
})");
    const auto team = data.get<Team>();
    EXPECT_EQ(team.number, "1234");
    EXPECT_EQ(team.name, "My Test Team");
    const std::vector<int> expected_scores = {0,100,200};
    EXPECT_THAT(team.scores, testing::ContainerEq(expected_scores));
    const std::vector<int> expected_gp_scores = {4, 3, 2};
    EXPECT_THAT(team.gp_scores, testing::ContainerEq(expected_gp_scores));
    EXPECT_EQ(team.display_score, 200);
    EXPECT_EQ(team.rank, 0);
}

TEST(JsonTeam, DumpTeam)
{
    const Team team{
        .number = "8765",
        .name = "Another Team",
        .scores = {400, 0, 150},
        .gp_scores = {2, 4, 3},
        .display_score = 400,
        .rank = 1
    };
    const nlohmann::json data = team;
    const nlohmann::json expected_data = {
            {"number", "8765"},
            {"name", "Another Team"},
            {"scores", {400,0,150}},
            {"gpScores", {2, 4, 3}},
            {"displayScore", 400},
            {"rank", 1}
    };
    EXPECT_EQ(data, expected_data);
}
