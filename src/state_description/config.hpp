#ifndef OLYMPUS_CONFIG_HPP
#define OLYMPUS_CONFIG_HPP

#include <string>

struct Config {
    bool show_timer{false};
    std::string competition_name;
    int rows_on_display{14};
    enum class DisplayState {
        ShowScores,
        Blackout,
        FllLogo,
        Sponsors
    } display_state = DisplayState::ShowScores;
    int display_seconds_per_page{5};
};

#endif //OLYMPUS_CONFIG_HPP
