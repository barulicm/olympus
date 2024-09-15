#ifndef OLYMPUS_SCORE_CALCULATOR_GENERATION_HPP
#define OLYMPUS_SCORE_CALCULATOR_GENERATION_HPP

#include <cpprest/details/basic_types.h>
#include "game_description/game.hpp"

namespace olympus::generation {

utility::string_t GenerateScoreCalculatorScript(const game_description::Game & game);

}

#endif //OLYMPUS_SCORE_CALCULATOR_GENERATION_HPP
