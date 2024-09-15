#ifndef QUESTION_HPP_
#define QUESTION_HPP_

#include <string>
#include <variant>

namespace olympus::game_description {

struct BoolQuestion {
    std::string description;
};

struct EnumQuestion {
    std::string description;
    std::vector<std::string> options;
};

struct NumberQuestion {
    std::string description;
    int min;
    int max;
    bool show_plus_on_max = false;
};

using Question = std::variant<BoolQuestion, EnumQuestion, NumberQuestion>;

}

#endif  // QUESTION_HPP_
