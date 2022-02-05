#ifndef OLYMPUS_CONFIG_H
#define OLYMPUS_CONFIG_H

#include <string>
#include <nlohmann/json.hpp>

struct Config {
    bool show_timer{false};
    std::string competition_name;
    int rows_on_display{8};

    [[nodiscard]] nlohmann::json ToJson() const;
    static Config FromJson(const nlohmann::json& json);
};

#endif //OLYMPUS_CONFIG_H
