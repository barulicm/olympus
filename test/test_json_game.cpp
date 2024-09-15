#include "game_description/json_game.hpp"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace olympus::game_description;

TEST(JsonGame, NoMissionsFromJson) {
    nlohmann::json data = nlohmann::json::parse(R"({"name":"TestName", "logo":"/path/to/logo.png", "missions":[]})");
    const auto game = data.get<Game>();
    EXPECT_EQ(game.name, "TestName");
    EXPECT_EQ(game.logo_path, "/path/to/logo.png");
    EXPECT_TRUE(game.missions.empty());
}

TEST(JsonGame, DefaultValidationFromJson) {
    nlohmann::json data = nlohmann::json::parse(R"(
{
    "name":"TestName",
    "logo":"/path/to/logo.png",
    "missions":[
        {
            "name": "Test Mission",
            "questions": [
                {
                    "type": "bool",
                    "description": "Test question description"
                }
            ],
            "scoring": "0"
        }
    ]
})");
    const auto game = data.get<Game>();
    EXPECT_EQ(game.name, "TestName");
    EXPECT_EQ(game.logo_path, "/path/to/logo.png");
    ASSERT_EQ(game.missions.size(), 1);
    const auto & mission = game.missions.front();
    EXPECT_EQ(mission.name, "Test Mission");
    EXPECT_EQ(mission.scoring_expression, "0");
    EXPECT_EQ(mission.validation_expression, "true");
    ASSERT_EQ(mission.questions.size(), 1);
    const auto & question_variant = mission.questions.front();
    ASSERT_TRUE(std::holds_alternative<BoolQuestion>(question_variant));
    const auto & question = std::get<BoolQuestion>(question_variant);
    EXPECT_EQ(question.description, "Test question description");
}

TEST(JsonGame, NonDefaultValidationFromJson) {
    nlohmann::json data = nlohmann::json::parse(R"(
{
    "name":"TestName2",
    "logo":"/path/to/logo2.png",
    "missions":[
        {
            "name": "Test Mission2",
            "questions": [
                {
                    "type": "bool",
                    "description": "Test question description2"
                }
            ],
            "scoring": "10",
            "validation": "false"
        }
    ]
})");
    const auto game = data.get<Game>();
    EXPECT_EQ(game.name, "TestName2");
    EXPECT_EQ(game.logo_path, "/path/to/logo2.png");
    ASSERT_EQ(game.missions.size(), 1);
    const auto & mission = game.missions.front();
    EXPECT_EQ(mission.name, "Test Mission2");
    EXPECT_EQ(mission.scoring_expression, "10");
    EXPECT_EQ(mission.validation_expression, "false");
    ASSERT_EQ(mission.questions.size(), 1);
    const auto & question_variant = mission.questions.front();
    ASSERT_TRUE(std::holds_alternative<BoolQuestion>(question_variant));
    const auto & question = std::get<BoolQuestion>(question_variant);
    EXPECT_EQ(question.description, "Test question description2");
}
