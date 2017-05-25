#include "Schedule.h"

const std::vector<Match> &Schedule::matches() {
    return _matches[_currentPhase];
}

void Schedule::nextPhase() {
    _currentPhase++;
}

int Schedule::currentPhase() {
    return _currentPhase;
}
