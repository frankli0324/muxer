#ifndef MUXER_UTILS_H
#define MUXER_UTILS_H

#include <string>
#include <vector>
#include <unordered_map>

namespace muxer::utils {

    std::string ltrim(const std::string &s, const std::string& trim_chars = "\r\n\t\f\v ");

    std::string rtrim(const std::string &s, const std::string& trim_chars = "\r\n\t\f\v ");

    std::string trim(const std::string &s, const std::string& trim_chars = "\r\n\t\f\v ");

    std::unordered_map<char, std::string>
    argparse(int argc, char *argv[], const char *options, const char *flags);
}

#endif //MUXER_UTILS_H
