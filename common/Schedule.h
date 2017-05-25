#ifndef OLYMPUS_SCHEDULE_H
#define OLYMPUS_SCHEDULE_H

#include <vector>
#include "Match.h"

class Schedule {
public:

    const std::vector<Match> &matches();

    void nextPhase();

    int currentPhase();

private:

    int _currentPhase = 0;
    std::vector<std::vector<Match>> _matches;

};


#endif //OLYMPUS_SCHEDULE_H
