#include "timer_handler.h"
#include <nlohmann/json.hpp>

std::vector<RequestHandlerDetails> TimerHandler::GetHandlers() {
    return {{
                    web::http::methods::GET,
                    std::bind_front(&TimerHandler::PathPredicateGet, this),
                    std::bind_front(&TimerHandler::CallbackGet, this)
            },
            {
                    web::http::methods::PUT,
                    std::bind_front(&TimerHandler::PathPredicatePut, this),
                    std::bind_front(&TimerHandler::CallbackPut, this)
            }};
}

bool TimerHandler::PathPredicateGet(const utility::string_t &path) {
    return path == "/timer";
}

bool TimerHandler::PathPredicatePut(const utility::string_t &path) {
    return path == "/timer/start" || path == "/timer/stop";
}

void TimerHandler::CallbackGet(const web::http::http_request &request) {
    nlohmann::json json_response = {
            {"time_remaining", timer_seconds_.load()}
    };
    request.reply(web::http::status_codes::OK, json_response.dump(), U("application/json")).wait();
}

void TimerHandler::CallbackPut(const web::http::http_request &request) {
    const auto path = request.relative_uri().path();
    if(path == "/timer/start") {
        StartTimer();
    } else if(path == "/timer/stop") {
        StopTimer();
    }
    request.reply(web::http::status_codes::OK).wait();
}

void TimerHandler::StartTimer() {
    timer_running_ = true;
    timer_thread_ = std::thread([this](){
        const auto stop_time = std::chrono::steady_clock::now() + std::chrono::seconds(timer_match_length_);
        while(timer_running_) {
            const auto seconds_remaining = std::chrono::duration_cast<std::chrono::seconds>(stop_time - std::chrono::steady_clock::now()).count();
            timer_seconds_ = std::max(0l, seconds_remaining);
            if(timer_seconds_ == 0) {
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    });
}

void TimerHandler::StopTimer() {
    if(!timer_running_)
        return;
    timer_running_ = false;
    timer_thread_.join();
    timer_seconds_ = timer_match_length_;
}
