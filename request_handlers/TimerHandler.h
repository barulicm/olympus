#ifndef OLYMPUS_TIMERHANDLER_H
#define OLYMPUS_TIMERHANDLER_H

#include <filesystem>
#include <thread>
#include "RequestHandler.h"

class TimerHandler : public RequestHandler {
public:

    std::vector<RequestHandlerDetails> GetHandlers() override;

private:
    static constexpr int timer_match_length_ = 150;
    std::atomic<int> timer_seconds_ = timer_match_length_;
    std::atomic<bool> timer_running_ = false;
    std::thread timer_thread_;

    const std::filesystem::path static_resources_dir = "resources/static/";
    bool PathPredicateGet(const utility::string_t& path);
    bool PathPredicatePut(const utility::string_t& path);
    void CallbackGet(const web::http::http_request& request);
    void CallbackPut(const web::http::http_request& request);
    void StartTimer();
    void StopTimer();

};

#endif //OLYMPUS_TIMERHANDLER_H
