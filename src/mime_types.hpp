#ifndef OLYMPUS_MIME_TYPES_H
#define OLYMPUS_MIME_TYPES_H

#include <filesystem>
#include <string>
#include <cpprest/details/basic_types.h>

utility::string_t GetMimeTypeForPath(const std::filesystem::path& path);

#endif //OLYMPUS_MIME_TYPES_H
