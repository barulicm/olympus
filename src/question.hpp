#ifndef QUESTION_HPP_
#define QUESTION_HPP_

#include <variant>
#include "bool_question.hpp"
#include "enum_question.hpp"
#include "number_question.hpp"

using Question = std::variant<BoolQuestion, NumberQuestion, EnumQuestion>;

#endif  // QUESTION_HPP_
