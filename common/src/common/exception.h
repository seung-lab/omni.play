#pragma once

/**
 * Exception handeling.
 *
 * om::Exception is the parent Exception class. Subclasses can be
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

class Exception : public std::exception {
public:
    Exception(const std::string& name,
                const std::string& msg)
        : fullMessage_(name + ": " + msg)
    {}

    virtual ~Exception() throw() {}

    virtual const char* what() const throw()
    {
        return fullMessage_.c_str();
    }

protected:
    const std::string fullMessage_;
};

class AccessException : public Exception {
public:
    AccessException(const std::string& msg)
        : Exception("AccessException", msg) { }
};

class ArgException : public Exception {
public:
    ArgException(const std::string& msg)
        : Exception("ArgException", msg) { }
};

class FormatException : public Exception {
public:
    FormatException(const std::string& msg)
        : Exception("FormatException", msg) { }
};

class IoException : public Exception {
public:
    IoException(const char* msg)
        : Exception("IoException", std::string(msg))
    {}
    IoException(const std::string& msg)
        : Exception("IoException", msg)
    {}
    IoException(const char* msg, const std::string& fnp)
        : Exception("IoException", str(boost::format("%1%: %2%") % fnp % msg))
    {}
};

class ModificationException : public Exception {
public:
    ModificationException(const std::string& msg)
        : Exception("ModificationException", msg) { }
};

class VerifyException : public Exception {
public:
    VerifyException(const std::string& msg)
        : Exception("VerifyException", msg) { }
};

} // namespace om
