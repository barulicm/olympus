#include "score_calculator_generation.hpp"
#include "utilities/string_utilities.hpp"

namespace olympus::generation {

utility::string_t GenerateScoreCalculatorScript(const olympus::game_description::Game &game) {
    utility::stringstream_t stream;

    stream << U("function calculateScore(");

    {
        int mission_index = 0;
        for (const auto &mission: game.missions) {
            const auto num_questions = std::min(mission.questions.size(), std::size_t{26});
            for (auto question_index = 0; question_index < num_questions; ++question_index) {
                utility::string_t question_id =
                        U("M") + utilities::ToUString(mission_index) + utilities::ToUString(char('a' + question_index));
                stream << question_id;
                if (question_index < (num_questions - 1) || mission_index < (game.missions.size() - 1)) {
                    stream << U(", ");
                }
            }
            mission_index++;
        }
    }

    stream << U(R"!!!()
{
    let score = 0;
)!!!");

    {
        int mission_index = 0;
        for (const auto &mission : game.missions) {
            stream << U("    {\n");
            const auto num_questions = std::min(mission.questions.size(), std::size_t{26});
            for(auto question_index = 0; question_index < num_questions; ++question_index) {
                const auto & question = mission.questions.at(question_index);
                const auto question_char = utilities::ToUString(char('a' + question_index));
                const auto js_question_id = U("M") + utilities::ToUString(mission_index) + question_char;
                const auto src_question_id = U("q") + question_char;
                stream << U("        let ") << src_question_id << U(" = ");
                if(std::holds_alternative<game_description::BoolQuestion>(question)) {
                    stream << js_question_id << U(" === \"y\";");
                } else if(std::holds_alternative<game_description::EnumQuestion>(question)) {
                    stream << js_question_id << U(";");
                } else if(std::holds_alternative<game_description::NumberQuestion>(question)) {
                    stream << U("parseInt(") << js_question_id << U(");");
                }
                stream << U("\n");
            }
            stream << U("        score += ") << utilities::ToUString(mission.scoring_expression) << U(";\n");
            stream << U("    }\n");
            mission_index++;
        }
    }

    stream << U(R"!!!(
    return score;
}
)!!!");

    return stream.str();
}

}
