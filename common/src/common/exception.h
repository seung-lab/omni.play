#pragma once

/**
 * Exception handeling.
 *
 * OmException is the parent exception class. Subclasses can be
 * formed as described in the example code below.
 *
 * Brett Warne - bwarne@mit.edu - 2/6/09
 */

#include "common/common.h"

#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include <cstdarg>
#include <exception>

namespace om {

class exception : public std::exception {
 public:
  exception(const std::string& name, const std::string& msg)
      : fullMessage_(name + ": " + msg) {}

  virtual ~exception() throw() {}

  virtual const char* what() const throw() { return fullMessage_.c_str(); }

 protected:
  const std::string fullMessage_;
};

class accessException : public exception {
 public:
  accessException(const std::string& msg) : exception("accessException", msg) {}
};

class argException : public exception {
 public:
  argException(const std::string& msg) : exception("argException", msg) {}
};

class formatException : public exception {
 public:
  formatException(const std::string& msg) : exception("formatException", msg) {}
};

class ioException : public exception {
 public:
  ioException(const char* msg) : exception("ioException", std::string(msg)) {}
  ioException(const std::string& msg) : exception("ioException", msg) {}
  ioException(const char* msg, const std::string& fnp)
      : exception("ioException", str(boost::format("%1%: %2%") % fnp % msg)) {}
};

class modificationException : public exception {
 public:
  modificationException(const std::string& msg)
      : exception("modificationException", msg) {}
};

class verifyException : public exception {
 public:
  verifyException(const std::string& msg) : exception("verifyException", msg) {}
};

}  // namespace om
