#ifndef MUXER_LOGGER_H
#define MUXER_LOGGER_H

#include <fstream>
#include <ostream>

#include <boost/log/trivial.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/core/core.hpp>
#include <boost/log/expressions/formatters/date_time.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

#include <boost/core/null_deleter.hpp>
#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>


BOOST_LOG_GLOBAL_LOGGER(logger, boost::log::sources::severity_logger_mt<boost::log::trivial::severity_level>)

#define _LOG(severity) BOOST_LOG_SEV(logger::get(), boost::log::trivial::severity)

// ===== log macros =====
#define TRACE   _LOG(trace)
#define DEBUG   _LOG(debug)
#define INFO    _LOG(info)
#define WARNING _LOG(warning)
#define ERROR   _LOG(error)
#define FATAL   _LOG(fatal)

#endif //MUXER_LOGGER_H
