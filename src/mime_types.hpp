#ifndef OLYMPUS_MIME_TYPES_H
#define OLYMPUS_MIME_TYPES_H

#include <filesystem>
#include <string>

std::string GetMimeTypeForPath(const std::filesystem::path& path);

#endif //OLYMPUS_MIME_TYPES_H
