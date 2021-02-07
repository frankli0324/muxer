#include "logger.h"

namespace muxer::log {
    namespace logging = boost::log;
    namespace src = boost::log::sources;
    namespace expr = boost::log::expressions;
    namespace sinks = boost::log::sinks;
    namespace attrs = boost::log::attributes;

    void colored(logging::record_view const &rec, logging::formatting_ostream &strm) {
        auto severity = rec[logging::trivial::severity];
        if (severity) {
            switch (severity.get()) {
                case logging::trivial::severity_level::info:
                    strm << "\033[32m";
                    break;
                case logging::trivial::severity_level::warning:
                    strm << "\033[33m";
                    break;
                case logging::trivial::severity_level::error:
                case logging::trivial::severity_level::fatal:
                    strm << "\033[31m";
                    break;
                default:
                    break;
            }
        }
        strm << logging::extract<boost::posix_time::ptime>("TimeStamp", rec)
             << ": <" << rec[logging::trivial::severity] << "> "
             << rec[expr::message];

        if (severity) {
            strm << "\033[0m";
        }
    }

}
using namespace muxer::log;
BOOST_LOG_ATTRIBUTE_KEYWORD(line_id, "LineID", unsigned int)
BOOST_LOG_ATTRIBUTE_KEYWORD(timestamp, "TimeStamp", boost::posix_time::ptime)
BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", logging::trivial::severity_level)

BOOST_LOG_GLOBAL_LOGGER_INIT(logger, src::severity_logger_mt) {
    src::severity_logger_mt<boost::log::trivial::severity_level> logger;

    logger.add_attribute("LineID", attrs::counter<unsigned int>(1)); // lines are sequentially numbered
    logger.add_attribute("TimeStamp", attrs::local_clock());               // each log line gets a timestamp

    typedef sinks::synchronous_sink<sinks::text_ostream_backend> text_sink;
    boost::shared_ptr<text_sink> sink = boost::make_shared<text_sink>();

//    sink->locked_backend()->add_stream(boost::make_shared<std::ofstream>(LOGFILE));
    sink->locked_backend()->add_stream(boost::shared_ptr<std::ostream>(&std::clog, boost::null_deleter()));

    sink->set_formatter(&colored);
    sink->set_filter(severity >= logging::trivial::debug);

    logging::core::get()->add_sink(sink);

    return logger;
}
