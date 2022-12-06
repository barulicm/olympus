#ifndef OLYMPUS_JSON_SESSION_HPP
#define OLYMPUS_JSON_SESSION_HPP

#include "session.hpp"
#include "config.hpp"
#include <nlohmann/json.hpp>

void to_json(nlohmann::json& json, const Session& session);
void from_json(const nlohmann::json& json, Session& session);

void to_json(nlohmann::json& json, const Config& config);
void from_json(const nlohmann::json& json, Config& config);

#endif //OLYMPUS_JSON_SESSION_HPP
