#ifndef NUMBER_QUESTION_HPP_
#define NUMBER_QUESTION_HPP_

#include <string>

struct NumberQuestion
{
  std::string description;
  int min;
  int max;
  bool show_plus_on_max = false;
};

#endif  // NUMBER_QUESTION_HPP_
