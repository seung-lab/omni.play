/*
 * AUTHORS:
 *   Aleksandar Zlateski <zlateski@mit.edu>
 *
 * Do not share without authors permission.
 */

#ifndef CONCURRENCY_EXCEPTION_H_
#define CONCURRENCY_EXCEPTION_H_

#include <string>
#include <exception>

namespace zi { namespace Threads {

class ZIException : public std::exception {
public:
  ZIException() {}
  ZIException(const std::string& message) : message_(message) {}
  virtual ~ZIException() throw() {}
  virtual const char* what() const throw() {
    if (message_.empty()) {
      return "Default Exception.";
    } else {
      return message_.c_str();
    }
  }
protected:
  std::string message_;
};

class TimedOutException : public ZIException {
public:
  TimedOutException():ZIException("TimedOutException"){};
  TimedOutException(const std::string& message ) : ZIException(message) {}
};

class ThreadException : public ZIException {
public:
  ThreadException():ZIException("Thread Exception"){};
  ThreadException(const std::string& message ) : ZIException(message) {}
};

} }

#endif
