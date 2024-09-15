#include "scorecard_generation.hpp"
#include <algorithm>
#include "utilities/string_utilities.hpp"

namespace olympus::generation {

const utility::string_t scorecard_prefix = U(R"!!!(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <link rel="stylesheet" type="text/css" href="stylesheets/stylesheet.css" media="screen" />
    <link rel="stylesheet" type="text/css" href="stylesheets/Scorecard.css" media="screen" />
    <script src="scripts/Scorecard.js"></script>
    <script src="ScoreCalculator.js"></script>
    <title>FLL Masterpiece Scorecard</title>
</head>
<body onload="onLoad()">
<div class="navigation">
    <a class="navigation" href="index.html">Home</a>
    <a class="navigation" href="teams.html">Teams</a>
    <a class="navigation" href="Scorecard.html">Scorecard</a>
    <a class="navigation" href="Display.html">Display</a>
    <a class="navigation" href="results.html">Results</a>
    <a class="navigation" href="TimerDisplay.html">Timer</a>
</div>
<img id="logo" src="game/logo" />
)!!!");

const utility::string_t form_prefix = U(R"!!!(
    <h1>Scorecard</h1>
    <table>
        <tbody>
        <tr>
            <td>
                <label for="team" class="grouptitle">Team</label>
                <input type="text" id="team" name="team" list="teams" onchange="onTeamChange(this.value)">
                <datalist id="teams">
                </datalist>
            </td>
            <td>
                <label class="grouptitle">Match</label>
                <output id="matchOutput">0</output>
            </td>
            <td>
                <input type="reset">
            </td>
        </tr>
)!!!");

const utility::string_t scorecard_suffix = U(R"!!!(
        </tbody>
    </table>
</form>
</body>
</html>
)!!!");

void GenerateQuestionHtml(const game_description::BoolQuestion &, const int &mission_index, const char &question_index,
                          utility::stringstream_t &stream) {
    utility::string_t question_id = U("M") + utilities::ToUString(mission_index) + utilities::ToUString(question_index);
    stream << U(R"!!!(
                <div class="switch-field">
                    <input type="radio" id=")!!!" << question_id << R"!!!(n" name=")!!!" << question_id << R"!!!(" value="n" checked>
                    <label for=")!!!" << question_id << R"!!!(n">No</label>
                    <input type="radio" id=")!!!" << question_id << R"!!!(y" name=")!!!" << question_id << R"!!!(" value="y">
                    <label for=")!!!" << question_id << R"!!!(y">Yes</label>
                </div>
)!!!");
}

void GenerateQuestionHtml(const game_description::EnumQuestion &question, const int &mission_index,
                          const char &question_index, utility::stringstream_t &stream) {
    utility::string_t question_id = U("M") + utilities::ToUString(mission_index) + utilities::ToUString(question_index);
    stream << U(R"!!!(
                <div class="switch-field">
)!!!");
    auto option_index = 0;
    for (const auto &option: question.options) {
        const auto option_id = question_id + utilities::ToUString(option_index);
        stream << U(R"!!!(
                    <input type="radio" name=")!!!" << question_id << R"!!!(" id=")!!!" << option_id
                                                    << R"!!!(" value=")!!!" << utilities::ToUString(option) << U("\"");
        if(option_index == 0) {
            stream << U(" checked");
        }
        stream << R"!!!(">
                    <label for=")!!!" << option_id << R"!!!(">)!!!" << utilities::ToUString(option) << R"!!!(</label>
)!!!");
        option_index++;
    }
    stream << U(R"!!!(
                </div>
)!!!");
}

void GenerateQuestionHtml(const game_description::NumberQuestion &question, const int &mission_index,
                          const char &question_index, utility::stringstream_t &stream) {
    utility::string_t question_id = U("M") + utilities::ToUString(mission_index) + utilities::ToUString(question_index);
    stream << U(R"!!!(
                <div class="switch-field">
)!!!");
    for (auto i = question.min; i <= question.max; ++i) {
        const auto option_id = question_id + utilities::ToUString(i);
        const auto value_text = utilities::ToUString(i);
        auto display_text = value_text;
        if (question.show_plus_on_max && i == question.max) {
            display_text += U("+");
        }
        stream << U(R"!!!(
                    <input type="radio" name=")!!!" << question_id << R"!!!(" id=")!!!" << option_id
                                                    << R"!!!(" value=")!!!" << value_text << U("\"");
        if(i == 0) {
            stream << U(" checked");
        }
        stream << R"!!!(>
                    <label for=")!!!" << option_id << R"!!!(">)!!!" << display_text << R"!!!(</label>
)!!!");
    }
    stream << U(R"!!!(
                </div>
)!!!");
}

void GenerateMissionHtml(const game_description::Mission &mission, const int &mission_index,
                         utility::stringstream_t &stream) {
    stream << U(R"!!!(
        <tr>
            <td colspan=3>
                <label class="grouptitle">)!!!");
    stream << utility::string_t(mission.name.begin(), mission.name.end());
    stream << U("</label>\n");
    char question_index = 'a';
    for (const auto &question: mission.questions) {
        if (question_index > 'z') {
            /* It's a completely arbitrary limit because I want to use letters and don't feel like implementing double+
             * letters right now.
             */
            std::cerr << "Too many questions in mission. Max supported is 26. Remaining questions will be ignored.\n";
            break;
        }
        std::visit([&mission_index, &question_index, &stream](const auto &q) {
            GenerateQuestionHtml(q, mission_index, question_index, stream);
        }, question);
        question_index++;
    }
    stream << U(R"!!!(
            </td>
        </tr>
)!!!");

}

void GenerateCalculateScoreCall(const game_description::Game &game, utility::stringstream_t &stream) {
    stream << U("calculateScore(");
    int mission_index = 0;
    for (const auto &mission: game.missions) {
        const auto num_questions = std::min(mission.questions.size(), std::size_t{26});
        for (auto question_index = 0; question_index < num_questions; ++question_index) {
            utility::string_t question_id =
                    U("M") + utilities::ToUString(mission_index) + utilities::ToUString(char('a' + question_index));
            stream << question_id << U(".value");
            if (question_index < (num_questions - 1) || mission_index < (game.missions.size() - 1)) {
                stream << U(", ");
            }
        }
        mission_index++;
    }
    stream << U(")");
}

utility::string_t GenerateScorecardHtml(const game_description::Game &game) {
    utility::stringstream_t html_stream;
    html_stream << scorecard_prefix;

    html_stream << U("<form class=\"form-style-7\" onchange=\"scoreOutput.value=");
    GenerateCalculateScoreCall(game, html_stream);
    html_stream << U("\">\n");
    html_stream << form_prefix;

    {
        auto mission_index = 0;
        for (const auto &mission: game.missions) {
            GenerateMissionHtml(mission, mission_index, html_stream);
            mission_index++;
        }
    }

    html_stream << U(R"!!!(
        <tr>
            <td colspan=3>
                <label class="grouptitle">GP - Gracious Professionalism</label>
                <div class="switch-field">
                    <input type="radio" name="GP" id="GP2" value="2">
                    <label for="GP2">Developing</label>
                    <input type="radio" name="GP" id="GP3" value="3" checked>
                    <label for="GP3">Accomplished</label>
                    <input type="radio" name="GP" id="GP4" value="4">
                    <label for="GP4">Exceeds</label>
                </div>
            </td>
        </tr>
        <tr>
            <td colspan=3>
                <label for="scoreOutput" class="grouptitle">Final Score</label>
                <output id="scoreOutput" name="scoreOutput">0</output>
            </td>
        </tr>
        <tr>
            <td colspan=3>
                <input type="button" class="button" id="submitButton" onclick="submitScore(team.value,)!!!");
    GenerateCalculateScoreCall(game, html_stream);
    html_stream << U(R"!!!(, GP.value)" value="Submit">
            </td>
        </tr>
)!!!");

    html_stream << scorecard_suffix;
    return html_stream.str();
}

}
