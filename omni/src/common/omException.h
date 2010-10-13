#ifndef OMEXCEPTION_H
#define OMEXCEPTION_H

/**
 *	Exception handeling for the Omni System.
 *
 *	OmException is the parent exception class. Subclasses can be
 *	formed as described in the example code below.
 *
 *	Brett Warne - bwarne@mit.edu - 2/6/09
 */

#include "common/omCommon.h"

#include <cstdarg>
#include <exception>

class OmException : public std::exception {
public:
	OmException(const std::string& name,
				const std::string& msg)
		: name_(name)
		, message_(msg)
		, fullMessage_()
	{}

	virtual ~OmException() throw() {}

	virtual const char* what() const throw()
	{
		if(fullMessage_.empty()){
			fullMessage_ = name_ + ": " + message_;
		}

		return fullMessage_.c_str();
	}

protected:
	const std::string name_;
	const std::string message_;
	mutable std::string fullMessage_;
};

class OmAccessException : public OmException {
public:
	OmAccessException(const std::string& msg)
		: OmException("OmAccessException", msg) { }
};

class OmArgException : public OmException {
public:
	OmArgException(const std::string& msg)
		: OmException("OmArgException", msg) { }
};

class OmFormatException : public OmException {
public:
	OmFormatException(const std::string& msg)
		: OmException("OmFormatException", msg) { }
};

class OmIoException : public OmException {
public:
	OmIoException(const std::string& msg)
		: OmException("OmIoException", msg) { }
};

class OmModificationException : public OmException {
public:
	OmModificationException(const std::string& msg)
		: OmException("OmModificationException", msg) { }
};

#endif
