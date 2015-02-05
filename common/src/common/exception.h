#pragma once

#include "precomp.h"

namespace om {

class Exception : public std::exception {
 public:
  Exception(const std::string& name, const std::string& msg)
      : fullMessage_(name + ": " + msg) {}

  virtual ~Exception() throw() {}

  virtual const char* what() const throw() { return fullMessage_.c_str(); }

 protected:
  const std::string fullMessage_;
};

class AccessException : public Exception {
 public:
  AccessException(const std::string& msg) : Exception("AccessException", msg) {}
};

class ArgException : public Exception {
 public:
  ArgException(const std::string& msg) : Exception("ArgException", msg) {}

  template <class T>
  ArgException(const std::string& msg, const T& s)
      : Exception("ArgException", msg + "; arg was: " + std::to_string(s)) {}
};

class FormatException : public Exception {
 public:
  FormatException(const std::string& msg) : Exception("FormatException", msg) {}
};

class IoException : public Exception {
 public:
  IoException(const char* msg) : Exception("IoException", std::string(msg)) {}
  IoException(const std::string& msg) : Exception("IoException", msg) {}
  IoException(const char* msg, const std::string& fnp)
      : Exception("IoException", str(boost::format("%1%: %2%") % fnp % msg)) {}
};

class ModificationException : public Exception {
 public:
  ModificationException(const std::string& msg)
      : Exception("ModificationException", msg) {}
};

class VerifyException : public Exception {
 public:
  VerifyException(const std::string& msg) : Exception("VerifyException", msg) {}
};

class InvalidOperationException : public Exception {
 public:
  InvalidOperationException(const std::string& msg)
      : Exception("InvalidOperationException", msg) {}
};

class NotImplementedException : public Exception {
 public:
  NotImplementedException() : Exception("NotImplementedException", "") {}
  NotImplementedException(const std::string& method)
      : Exception("NotImplementedException", method) {}
};

}  // namespace om
