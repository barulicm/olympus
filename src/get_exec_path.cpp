#include "get_exec_path.hpp"

#include <windows.h>
#include <exception>

std::filesystem::path GetCurrentExecutableDirectory()
{
#ifdef _WIN32
    wchar_t buffer[MAX_PATH] = { 0 };
    GetModuleFileNameW( NULL, buffer, MAX_PATH );
    std::filesystem::path path(buffer);
    path.remove_filename();
    return path;
#elif __unix__
    return std::filesystem::canonical("/proc/self/exe").remove_filename();
#else
    throw std::runtime_error("GetCurrentExecutableDirectory() not implemented for current OS type.");
#endif
}