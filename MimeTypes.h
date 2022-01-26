#ifndef OLYMPUS_MIMETYPES_H
#define OLYMPUS_MIMETYPES_H

#include <filesystem>
#include <string>

std::string GetMimeTypeForPath(const std::filesystem::path& path);

#endif //OLYMPUS_MIMETYPES_H
