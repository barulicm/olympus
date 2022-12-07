#include "get_exec_path.hpp"

#include <exception>

std::filesystem::path GetCurrentExecutableDirectory()
{
#ifdef _WIN32
    throw std::runtime_error("GetCurrentExecutableDirectory() not implemented for Windows yet.");
#elif __unix__
    return std::filesystem::canonical("/proc/self/exe").remove_filename();
#else
    throw std::runtime_error("GetCurrentExecutableDirectory() not implemented for current OS type.");
#endif
}