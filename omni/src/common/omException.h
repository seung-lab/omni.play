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
		: fullMessage_(name + ": " + msg)
	{}

	virtual ~OmException() throw() {}

	virtual const char* what() const throw()
	{
		return fullMessage_.c_str();
	}

protected:
	const std::string fullMessage_;
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
	OmArgException(const QString& str1,
				   const QString& str2)
		: OmException("OmArgException",
					  QString(str1+" "+str2).toStdString())
	{}
};

class OmFormatException : public OmException {
public:
	OmFormatException(const std::string& msg)
		: OmException("OmFormatException", msg) { }
};

class OmIoException : public OmException {
public:
	OmIoException(const char* msg)
		: OmException("OmIoException", std::string(msg))
	{}
	OmIoException(const std::string& msg)
		: OmException("OmIoException", msg)
	{}
	OmIoException(const QString& str)
		: OmException("OmIoException",
					  str.toStdString())
	{}
	OmIoException(const QString& str1,
				  const QString& str2)
		: OmException("OmIoException",
					  QString(str1+" "+str2).toStdString())
	{}
	OmIoException(const QString& str1,
				  const std::string& str2)
		: OmException("OmIoException",
					  str1.toStdString()+" "+str2)
	{}
};

class OmModificationException : public OmException {
public:
	OmModificationException(const std::string& msg)
		: OmException("OmModificationException", msg) { }
};

class OmVerifyException : public OmException {
public:
	OmVerifyException(const std::string& msg)
		: OmException("OmVerifyException", msg) { }
};

#endif
