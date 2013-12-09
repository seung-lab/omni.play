#include "common/logging.h"
#include <boost/log/common.hpp>
#include <boost/log/core.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/utility/empty_deleter.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/attributes/clock.hpp>
#include <boost/log/attributes/current_thread_id.hpp>
#include <boost/log/expressions/attr.hpp>
#include <boost/log/expressions/filter.hpp>
#include <boost/log/expressions/predicates/is_in_range.hpp>
#include <fstream>

using namespace boost::log;

namespace om {
namespace logging {

void log_formatter(record_view const& rec, formatting_ostream& strm) {
  strm << extract<attributes::current_thread_id::value_type>("ThreadID", rec)
       << ": ";
  strm << "[" << extract<severity_level>("Severity", rec) << "] ";
  strm << rec[expressions::smessage];
}

void initLogging(std::string logfile, bool consoleLog) {
  boost::shared_ptr<core> c = core::get();
  c->add_global_attribute("TimeStamp", attributes::local_clock());
  c->add_global_attribute("ThreadID", attributes::current_thread_id());

  // Create a backend and attach a couple of streams to it
  boost::shared_ptr<sinks::text_ostream_backend> backend =
      boost::make_shared<sinks::text_ostream_backend>();
  if (consoleLog) {
    backend->add_stream(
        boost::shared_ptr<std::ostream>(&std::clog, empty_deleter()));
  }
  if (!logfile.empty()) {
    backend->add_stream(
        boost::shared_ptr<std::ostream>(new std::ofstream(logfile)));
  }

  // Enable auto-flushing after each log record written
  backend->auto_flush(true);

  // Wrap it into the frontend and register in the core.
  // The backend requires synchronization in the frontend.
  typedef sinks::synchronous_sink<sinks::text_ostream_backend> sink_t;
  boost::shared_ptr<sink_t> sink(new sink_t(backend));
  sink->set_formatter(&log_formatter);
  c->add_sink(sink);

#ifdef DEBUG_MODE
  c->set_filter(expressions::is_in_range(severity, debug, error));
#else
  c->set_filter(expressions::is_in_range(severity, info, error));
#endif
}

std::ostream& operator<<(std::ostream& s, severity_level l) {
  switch (l) {
    case debug:
      return s << "debug";
    case info:
      return s << "info";
    case error:
      return s << "error";
  }
  return s;
}
}
}  // namespace om::logging::
