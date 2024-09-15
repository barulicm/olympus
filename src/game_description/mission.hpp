#ifndef MISSION_HPP_
#define MISSION_HPP_

#include <string>
#include <vector>
#include "questions.hpp"

namespace olympus::game_description {

struct Mission {
    std::string name;
    std::vector<Question> questions;
    std::string scoring_expression;
    std::string validation_expression = "true";
};

}

#endif  // MISSION_HPP_
