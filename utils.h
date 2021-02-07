#ifndef MUXER_UTILS_H
#define MUXER_UTILS_H

#include <string>

namespace muxer::utils {
    std::string ltrim(const std::string &s, const std::string trim_chars = "\r\n\t\f\v ");

    std::string rtrim(const std::string &s, const std::string trim_chars = "\r\n\t\f\v ");

    std::string trim(const std::string &s, const std::string trim_chars = "\r\n\t\f\v ");
}

#endif //MUXER_UTILS_H
