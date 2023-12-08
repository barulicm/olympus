#include "mime_types.hpp"

#include <unordered_map>

std::unordered_map<std::string, utility::string_t> mime_types = {
        {".html",U("text/html")},
        {".css",U("text/css")},
        {".js",U("text/javascript")},
        {".bmp",U("image/bmp")},
        {".gif",U("image/gif")},
        {".jpg",U("image/jpeg")},
        {".png",U("image/png")},
        {".txt",U("text/plain")},
        {".csv",U("text/csv")}
};

utility::string_t GetMimeTypeForPath(const std::filesystem::path &path) {
    if(!mime_types.contains(path.extension().string())) {
        return U("text/plain");
    } else {
        return mime_types[path.extension().string()];
    }
}
