#include "timer_handler.hpp"
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
    return path == U("/timer");
}

bool TimerHandler::PathPredicatePut(const utility::string_t &path) {
    return path == U("/timer/start") || path == U("/timer/stop");
}

void TimerHandler::CallbackGet(const web::http::http_request &request) {
    nlohmann::json json_response = {
            {"time_remaining", timer_seconds_.load()}
    };
    const auto json_dump = json_response.dump();
    request.reply(web::http::status_codes::OK, utility::string_t(json_dump.begin(), json_dump.end()), U("application/json")).wait();
}

void TimerHandler::CallbackPut(const web::http::http_request &request) {
    const auto path = request.relative_uri().path();
    if(path == U("/timer/start")) {
        StartTimer();
    } else if(path == U("/timer/stop")) {
        StopTimer();
    }
    request.reply(web::http::status_codes::OK).wait();
}

void TimerHandler::StartTimer() {
    timer_running_ = true;
    timer_thread_ = std::thread([this](){
        const auto stop_time = std::chrono::steady_clock::now() + std::chrono::seconds(timer_match_length_);
        while(timer_running_) {
            const uint64_t seconds_remaining = std::chrono::duration_cast<std::chrono::seconds>(stop_time - std::chrono::steady_clock::now()).count();
            timer_seconds_ = std::max((uint64_t)0, seconds_remaining);
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
