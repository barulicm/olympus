#include "mime_types.hpp"

#include <unordered_map>

std::unordered_map<std::string, std::string> mime_types = {
        {".html","text/html"},
        {".css","text/css"},
        {".js","text/javascript"},
        {".bmp","image/bmp"},
        {".gif","image/gif"},
        {".jpg","image/jpeg"},
        {".png","image/png"},
        {".txt","text/plain"},
        {".csv", "text/csv"}
};

std::string GetMimeTypeForPath(const std::filesystem::path &path) {
    if(!mime_types.contains(path.extension())) {
        return "text/plain";
    } else {
        return mime_types[path.extension()];
    }
}
