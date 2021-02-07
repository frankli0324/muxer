#include "utils.h"

namespace muxer::utils {
    std::string ltrim(const std::string &s, const std::string trim_chars) {
        size_t start = s.find_first_not_of(trim_chars);
        return (start == std::string::npos) ? "" : s.substr(start);
    }

    std::string rtrim(const std::string &s, const std::string trim_chars) {
        size_t end = s.find_last_not_of(trim_chars);
        return (end == std::string::npos) ? "" : s.substr(0, end + 1);
    }

    std::string trim(const std::string &s, const std::string trim_chars) {
        return rtrim(ltrim(s, trim_chars), trim_chars);
    }
}