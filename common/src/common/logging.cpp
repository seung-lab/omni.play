#include "precomp.h"
#include "common/logging.h"

using namespace boost::log;

namespace om {
namespace logging {

size_t LOG_LIMIT;
bool printWithColors_;

void log_formatter(record_view const& rec, formatting_ostream& strm) {
  auto severity = extract<severity_level>("Severity", rec);

  if (printWithColors_)  {
    if(severity == om::logging::debug){
      strm << "\033[93m" ; //Yellow
    }else if(severity == om::logging::info){
      strm << "\033[94m" ; //Blue
    }else if(severity == om::logging::error){
      strm << "\033[91m" ; //Red
    }
  }
  strm << extract<attributes::current_thread_id::value_type>("ThreadID", rec)
       << ": ";
  strm << "[" << severity << "] ";
  if (LOG_LIMIT) {
    auto msg = extract<std::string>("Message", rec);
    if (!msg) {
      return;
    }
    auto message = msg.get();
    if (message.size() > LOG_LIMIT) {
      strm << message.substr(0, LOG_LIMIT) << "...";
      return;
    }
  }
  strm << rec[expressions::smessage];

  if (printWithColors_)  strm << "\033[0m"; //End color

}

void initLogging(std::string logfile, bool consoleLog, size_t logLimit, bool printWithColors) {
  LOG_LIMIT = logLimit;
  printWithColors_ = printWithColors;

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
  c->set_filter(expressions::is_in_range(
      severity, debug, (om::logging::severity_level)(error + 1)));
#else
  c->set_filter(expressions::is_in_range(
      severity, info, (om::logging::severity_level)(error + 1)));
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
