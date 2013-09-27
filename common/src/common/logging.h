#pragma once

#include <boost/log/common.hpp>
#include <boost/log/sources/severity_channel_logger.hpp>
#include <boost/log/expressions/keyword.hpp>
#include <boost/format.hpp>

namespace om {
namespace logging {

void initLogging(std::string logfile, bool consoleLog = true);

inline boost::format& my_printf_core(boost::format& format) { return format; }

template <typename T, typename... TArgs>
boost::format& my_printf_core(boost::format& format, T arg, TArgs... args) {
  return my_printf_core(format % arg, args...);
}

template <typename... TArgs>
std::string my_printf(const char* format, TArgs... args) {
  boost::format f(format);
  return str(my_printf_core(f, args...));
}

enum severity_level {
  debug,
  info,
  error
};

typedef boost::log::sources::severity_channel_logger_mt<
    severity_level, std::string> my_logger_mt;

BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", severity_level);
BOOST_LOG_INLINE_GLOBAL_LOGGER_INIT(logger, my_logger_mt) {
  return my_logger_mt(boost::log::keywords::channel = "general");
}
}
}  // namespace om::logging::

#define log_debugs(x) \
  BOOST_LOG_CHANNEL_SEV(om::logging::logger::get(), #x, om::logging::debug)
#define log_debug(x, ...)                                                   \
  BOOST_LOG_CHANNEL_SEV(om::logging::logger::get(), #x, om::logging::debug) \
      << om::logging::my_printf(__VA_ARGS__)

#define log_infos(x) \
  BOOST_LOG_CHANNEL_SEV(om::logging::logger::get(), #x, om::logging::info)
#define log_info(x, ...)                                                   \
  BOOST_LOG_CHANNEL_SEV(om::logging::logger::get(), #x, om::logging::info) \
      << om::logging::my_printf(__VA_ARGS__)

#define log_errors(x)                                                       \
  BOOST_LOG_CHANNEL_SEV(om::logging::logger::get(), #x, om::logging::error) \
      << __FILE__ << ":" << __LINE__ << ":" << BOOST_CURRENT_FUNCTION << ": "
#define log_error(x, ...)                                                   \
  BOOST_LOG_CHANNEL_SEV(om::logging::logger::get(), #x, om::logging::error) \
      << om::logging::my_printf(__VA_ARGS__)

#define log_variable(var) log_debugs(variable) << #var << "=" << var;