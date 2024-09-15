#ifndef OLYMPUS_SCORECARD_GENERATION_HPP
#define OLYMPUS_SCORECARD_GENERATION_HPP

#include <cpprest/details/basic_types.h>
#include "game_description/game.hpp"

namespace olympus::generation {

utility::string_t GenerateScorecardHtml(const game_description::Game & game);

}

#endif //OLYMPUS_SCORECARD_GENERATION_HPP
