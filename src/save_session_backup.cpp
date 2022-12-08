#include "save_session_backup.hpp"
#include <nlohmann/json.hpp>
#include "json_session.hpp"
#include <fstream>

void SaveSessionBackup(const Session& session) {
    time_t t = std::time(nullptr);   // get time now
    struct tm * now = std::localtime( & t );
    char filename [80];
    std::strftime (filename,80,"session-backup-%Y-%m-%d-%H-%M-%S.json",now);
    std::ofstream file(filename);
    file << nlohmann::json(session);
}
