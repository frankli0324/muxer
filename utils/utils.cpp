#include "utils.h"

namespace muxer::utils {
    std::string ltrim(const std::string &s, const std::string &trim_chars) {
        size_t start = s.find_first_not_of(trim_chars);
        return (start == std::string::npos) ? "" : s.substr(start);
    }

    std::string rtrim(const std::string &s, const std::string &trim_chars) {
        size_t end = s.find_last_not_of(trim_chars);
        return (end == std::string::npos) ? "" : s.substr(0, end + 1);
    }

    std::string trim(const std::string &s, const std::string &trim_chars) {
        return rtrim(ltrim(s, trim_chars), trim_chars);
    }

    std::unordered_map<char, std::string>
    argparse(int argc, char *argv[], const char *options, const char *flags) {
        std::unordered_map<char, std::string> args;
        std::string _options(options), _flags(flags);
        if (_options.length() * 2 + _flags.length() > argc - 1) {
            printf("usage");
            exit(-1);
        }
        for (int i = 0; i < argc; i++) {
            if (argv[i][0] == '-') {
                for (char *o = (char *) options; *o; o++) {
                    if (argv[i][1] == *o) {
                        args[*o] = argv[++i];
                        break;
                    }
                }
                for (char *o = (char *) flags; *o; o++) {
                    if (argv[i][1] == *o) {
                        args[*o] = "true";
                        break;
                    }
                }
            }
        }
        for (char *o = (char *) options; *o; o++)
            if (!args.contains(*o))args[*o] = "";

        for (char *o = (char *) options; *o; o++)
            if (!args.contains(*o))args[*o] = "false";
        return args;
    }
}