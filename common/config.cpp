#include "config.h"

nlohmann::json Config::ToJson() const {
    nlohmann::json json_rep;

    json_rep["show_timer"] = show_timer;
    json_rep["competition_name"] = competition_name;
    json_rep["rows_on_display"] = rows_on_display;

    return json_rep;
}

Config Config::FromJson(const nlohmann::json &json) {
    Config config;
    config.show_timer = json["show_timer"];
    config.competition_name = json["competition_name"];
    config.rows_on_display = json["rows_on_display"];
    return config;
}
