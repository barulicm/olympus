#ifndef OLYMPUS_GAME_DEFINITION_HANDLER_HPP
#define OLYMPUS_GAME_DEFINITION_HANDLER_HPP

#include "game_description/game.hpp"
#include "state_description/session.hpp"
#include "request_handler.hpp"

class GameDefinitionHandler : public RequestHandler {
public:
    explicit GameDefinitionHandler(Session & session, std::filesystem::path game_config_dir);

    std::vector<RequestHandlerDetails> GetHandlers() override;

private:
    Session & session_;
    std::filesystem::path game_config_dir_;
    std::vector<olympus::game_description::Game> available_games_;

    void LoadAvailableGames();
    void CallbackGet(const web::http::http_request& request);
    void CallbackPut(web::http::http_request request);
};


#endif //OLYMPUS_GAME_DEFINITION_HANDLER_HPP
