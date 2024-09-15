#ifndef OLYMPUS_STRING_UTILITIES_HPP
#define OLYMPUS_STRING_UTILITIES_HPP

#include <cpprest/details/basic_types.h>

namespace olympus::utilities
{

inline utility::string_t ToUString(const std::string & s) {
    return utility::string_t{s.begin(), s.end()};
}

inline utility::string_t ToUString(const int i) {
#ifdef _UTF16_STRINGS
    return std::to_wstring(i);
#else
    return std::to_string(i);
#endif
}

inline utility::string_t ToUString(const double & d) {
#ifdef _UTF16_STRINGS
    return std::to_wstring(d);
#else
    return std::to_string(d);
#endif
}

inline utility::string_t ToUString(const char & c) {
    return utility::string_t(1, c);
}

inline void ReplaceSubstring(utility::string_t & s, const utility::string_t & old_sub, const utility::string_t & new_sub) {
    size_t index = 0;
    while (true) {
        index = s.find(old_sub, index);
        if (index == std::string::npos) break;
        s.replace(index, old_sub.size(), new_sub);
        index += new_sub.size();
    }
}

}

#endif //OLYMPUS_STRING_UTILITIES_HPP
