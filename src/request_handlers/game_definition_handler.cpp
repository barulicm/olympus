#include "game_definition_handler.hpp"
#include <nlohmann/json.hpp>
#include <cpprest/filestream.h>
#include "mime_types.hpp"
#include "game_description/json_game.hpp"

GameDefinitionHandler::GameDefinitionHandler(Session &session, std::filesystem::path game_config_dir)
        : session_(session),
          game_config_dir_(game_config_dir) {
    LoadAvailableGames();
}

std::vector<RequestHandlerDetails> GameDefinitionHandler::GetHandlers() {
    auto path_predicate = [](const utility::string_t &path) {
        return path.starts_with(U("/game/"));
    };
    return {{
                    web::http::methods::GET,
                    path_predicate,
                    std::bind_front(&GameDefinitionHandler::CallbackGet, this)
            },
            {
                    web::http::methods::PUT,
                    path_predicate,
                    std::bind_front(&GameDefinitionHandler::CallbackPut, this)
            }};
}

void GameDefinitionHandler::LoadAvailableGames() {
    for (const auto &dir_entry: std::filesystem::directory_iterator{game_config_dir_}) {
        if (!dir_entry.is_regular_file()) {
            continue;
        }
        if (dir_entry.path().extension() != ".json") {
            continue;
        }
        const auto full_path = game_config_dir_ / dir_entry.path();
        available_games_.push_back(
                nlohmann::json::parse(std::ifstream{full_path}).get<olympus::game_description::Game>());
    }
}

void GameDefinitionHandler::CallbackGet(const web::http::http_request &request) {
    if (request.relative_uri().path() == U("/game/available_games")) {
        nlohmann::json response_body = nlohmann::json::array();
        for (const auto &game: available_games_) {
            nlohmann::json game_details = nlohmann::json::object();
            game_details["name"] = game.name;
            game_details["description"] = game.description;
            response_body.push_back(game_details);
        }
        const auto json_dump = response_body.dump();
        request.reply(web::http::status_codes::OK, utility::string_t(json_dump.begin(), json_dump.end()),
                      U("application/json")).wait();
        return;
    }

    if (request.relative_uri().path() == U("/game/meta")) {
        if(!session_.game) {
            request.reply(web::http::status_codes::NoContent, U("No game selected")).wait();
            return;
        }
        nlohmann::json response_body;
        response_body["name"] = session_.game->name;
        response_body["description"] = session_.game->description;
        const auto json_dump = response_body.dump();
        request.reply(web::http::status_codes::OK, utility::string_t(json_dump.begin(), json_dump.end()), U("application/json")).wait();
        return;
    }

    if (request.relative_uri().path() == U("/game/logo")) {
        if(!session_.game) {
            request.reply(web::http::status_codes::NoContent, U("No game selected")).wait();
            return;
        }
        const auto logo_file_path = game_config_dir_ / session_.game->logo_path;
        if(!std::filesystem::exists(logo_file_path)) {
            request.reply(web::http::status_codes::NotFound).wait();
            return;
        }
        concurrency::streams::fstream::open_istream(logo_file_path, std::ios::in).then([this, &request, &logo_file_path](const concurrency::streams::istream &is) {
            request.reply(web::http::status_codes::OK, is, GetMimeTypeForPath(logo_file_path)).wait();
        }).wait();
    }

    request.reply(web::http::status_codes::NotFound, U("Unrecognized game control URL")).wait();
}

void GameDefinitionHandler::CallbackPut(web::http::http_request request) {
    if(request.relative_uri().path() == U("/game/choose")) {
        request.extract_string().then([this, &request](const utility::string_t& body){
            auto by_name = [&body](const auto & game){
                return utility::string_t(game.name.begin(), game.name.end()) == body;
            };
            const auto found_iter = std::find_if(available_games_.begin(), available_games_.end(), by_name);
            if(found_iter == available_games_.end()) {
                request.reply(web::http::status_codes::BadRequest, U("No game with that name")).wait();
                return;
            }
            session_.game = *found_iter;
            request.reply(web::http::status_codes::OK);
        }).wait();
    }
}
