#include "generated_resource_handler.hpp"
#include "generation/scorecard_generation.hpp"
#include "generation/score_calculator_generation.hpp"

GeneratedResourceHandler::GeneratedResourceHandler(Session &session)
        : session_(session) {
}

std::vector<RequestHandlerDetails> GeneratedResourceHandler::GetHandlers() {
    return {
            {web::http::methods::GET,
             std::bind_front(&GeneratedResourceHandler::PathPredicate, this),
             std::bind_front(&GeneratedResourceHandler::Callback, this)}
    };
}

bool GeneratedResourceHandler::PathPredicate(const utility::string_t &path) {
    return session_.game && (path == U("/Scorecard.html") || path == U("/ScoreCalculator.js"));
}

void GeneratedResourceHandler::Callback(const web::http::http_request &request) {
    if (request.relative_uri().path() == U("/Scorecard.html")) {
        const auto html_body = olympus::generation::GenerateScorecardHtml(*session_.game);
        request.reply(web::http::status_codes::OK, html_body, U("text/html"));
        return;
    }
    if (request.relative_uri().path() == U("/ScoreCalculator.js")) {
        const auto html_body = olympus::generation::GenerateScoreCalculatorScript(*session_.game);
        request.reply(web::http::status_codes::OK, html_body, U("text/javascript"));
        return;
    }
}
