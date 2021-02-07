#ifndef MUXER_UTILS_H
#define MUXER_UTILS_H

#include <string>
#include <vector>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

namespace muxer::utils {

    std::string ltrim(const std::string &s, std::string trim_chars = "\r\n\t\f\v ");

    std::string rtrim(const std::string &s, std::string trim_chars = "\r\n\t\f\v ");

    std::string trim(const std::string &s, std::string trim_chars = "\r\n\t\f\v ");

    boost::property_tree::ptree parse_configs(const std::string &filename);
}

#endif //MUXER_UTILS_H
