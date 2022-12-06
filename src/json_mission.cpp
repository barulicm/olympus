#include "json_mission.hpp"

void to_json(nlohmann::json &json, const Mission &mission) {
    json = {
            {"name",      mission.name},
            {"questions", mission.questions},
            {"scoring", mission.scoring_expression},
            {"validation", mission.validation_expression}
    };
}

void from_json(const nlohmann::json &json, Mission &mission) {
    json.at("name").get_to(mission.name);
    json.at("questions").get_to(mission.questions);
    if(json.contains("scoring")) {
        json.at("scoring").get_to(mission.scoring_expression);
    }
    if(json.contains("validation")) {
        json.at("validation").get_to(mission.validation_expression);
    }
}

void to_json(nlohmann::json &json, const Question &question) {
    if (std::holds_alternative<BoolQuestion>(question)) {
        to_json(json, std::get<BoolQuestion>(question));
        json["type"] = "bool";
    } else if (std::holds_alternative<NumberQuestion>(question)) {
        to_json(json, std::get<NumberQuestion>(question));
        json["type"] = "number";
    } else if (std::holds_alternative<EnumQuestion>(question)) {
        to_json(json, std::get<EnumQuestion>(question));
        json["type"] = "enum";
    } else {
        // TODO throw exception
    }
}

void from_json(const nlohmann::json &json, Question &question) {
    const auto type_name = json.at("type").get<std::string>();
    if (type_name == "bool") {
        BoolQuestion bool_question;
        from_json(json, bool_question);
        question = bool_question;
    } else if (type_name == "number") {
        NumberQuestion number_question;
        from_json(json, number_question);
        question = number_question;
    } else if (type_name == "enum") {
        EnumQuestion enum_question;
        from_json(json, enum_question);
        question = enum_question;
    } else {
        // TODO throw exception
    }
}

void to_json(nlohmann::json &json, const BoolQuestion &question) {
    json = {
            {"description", question.description}
    };
}

void from_json(const nlohmann::json &json, BoolQuestion &question) {
    json.at("description").get_to(question.description);
}

void to_json(nlohmann::json &json, const NumberQuestion &question) {
    json = {
            {"description", question.description},
            {"min", question.min},
            {"max", question.max},
            {"show_plus_on_max", question.show_plus_on_max}
    };
}

void from_json(const nlohmann::json &json, NumberQuestion &question) {
    json.at("description").get_to(question.description);
    if (json.contains("min")) {
        json.at("min").get_to(question.min);
    }
    if (json.contains("max")) {
        json.at("max").get_to(question.max);
    }
    if (json.contains("show_plus_on_max")) {
        json.at("show_plus_on_max").get_to(question.show_plus_on_max);
    }
}

void to_json(nlohmann::json &json, const EnumQuestion &question) {
    json = {
            {"description", question.description},
            {"options", question.options}
    };
}

void from_json(const nlohmann::json &json, EnumQuestion &question) {
    json.at("description").get_to(question.description);
    json.at("options").get_to(question.options);
}
