#include "game_description/json_mission.hpp"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace olympus::game_description;

TEST(JsonMission, MissionNameFromJson) {
    nlohmann::json data = nlohmann::json::parse(R"({"name":"M10", "questions":[], "scoring": "0"})");
    const auto mission = data.get<Mission>();
    EXPECT_EQ(mission.name, "M10");
}

TEST(JsonMission, BoolQuestionFromJson) {
    nlohmann::json data = nlohmann::json::parse(R"({
    "type": "bool",
    "description": "This is the bool description"
})");
    const auto question = data.get<Question>();
    ASSERT_TRUE(std::holds_alternative<BoolQuestion>(question));
    const auto &bool_question = std::get<BoolQuestion>(question);
    EXPECT_EQ(bool_question.description, "This is the bool description");
}

TEST(JsonMission, NumberQuestionFromJson) {
    nlohmann::json data = nlohmann::json::parse(R"({
    "type": "number",
    "description": "This is the number description",
    "min": 0,
    "max": 3
})");
    const auto question = data.get<Question>();
    ASSERT_TRUE(std::holds_alternative<NumberQuestion>(question));
    const auto &number_question = std::get<NumberQuestion>(question);
    EXPECT_EQ(number_question.description, "This is the number description");
    EXPECT_EQ(number_question.min, 0);
    EXPECT_EQ(number_question.max, 3);
    EXPECT_EQ(number_question.show_plus_on_max, false) << " show_plus_on_max should default to false.";
}

TEST(JsonMission, EnumQuestionFromJson) {
    nlohmann::json data = nlohmann::json::parse(R"({
    "type": "enum",
    "description": "This is the enum description",
    "options": ["Completely", "Partially", "Not at all"]
})");
    const auto question = data.get<Question>();
    ASSERT_TRUE(std::holds_alternative<EnumQuestion>(question));
    const auto &enum_question = std::get<EnumQuestion>(question);
    EXPECT_EQ(enum_question.description, "This is the enum description");
    const std::vector<std::string> expected_options = {"Completely", "Partially", "Not at all"};
    EXPECT_THAT(enum_question.options, testing::ContainerEq(expected_options));
}

TEST(JsonMission, MultipleMissionsAllQuestionTypes) {
    nlohmann::json data = nlohmann::json::parse(R"(
{
  "missions": [
    {
      "name": "M00",
      "questions": [
        {
          "type": "bool",
          "description": "Question M00.1"
        }
      ],
      "scoring": "0"
    },
    {
      "name": "M01",
      "questions": [
        {
          "type": "number",
          "description": "Question M01.1",
          "min": 0,
          "max": 6
        },
        {
            "type": "enum",
            "description": "Question M01.2",
            "options": [
                "Opt 1",
                "Opt 2",
                "Opt 3"
            ]
        }
      ],
      "scoring": "0"
    }
  ]
}
)");

    const std::vector<Mission> missions = data.at("missions").get<std::vector<Mission >>();

    ASSERT_EQ(missions.size(), 2);

}