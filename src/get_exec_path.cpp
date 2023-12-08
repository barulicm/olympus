#include "get_exec_path.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

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