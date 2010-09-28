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

#include <stdarg.h>
#include <exception>

class OmException : public std::exception {
public:
	OmException(const std::string &name,
				const std::string &msg)
		: mName(name),
		  mMessage(msg),
		  message_()
	{}

	virtual ~OmException() throw() {}

	virtual const char* what() const throw()
	{
		if ( message_.empty() )
		{
			message_ = mName + ": " + mMessage;
		}
		return message_.c_str();
	}

protected:
	std::string mName;
	std::string mMessage;
	mutable std::string message_;
};

class OmAccessException : public OmException {
public:
	OmAccessException(std::string msg)
	: OmException("OmAccessException", msg) { }
};

class OmArgException : public OmException {
public:
	OmArgException(std::string msg)
	: OmException("OmArgException", msg) { }
};

class OmFormatException : public OmException {
public:
	OmFormatException(std::string msg)
	: OmException("OmFormatException", msg) { }
};

class OmIoException : public OmException {
public:
	OmIoException(std::string msg)
	: OmException("OmIoException", msg) { }
};

class OmModificationException : public OmException {
public:
	OmModificationException(std::string msg)
	: OmException("OmModificationException", msg) { }
};

#endif
