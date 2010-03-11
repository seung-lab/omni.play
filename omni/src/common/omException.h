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

#include "common/omStd.h"
#include <stdarg.h>

enum OmExceptType {  EXCEPT_NOTE = 0, // not reported to user, make internal note
		     EXCEPT_INFO,     // give user information (usage, formatting, etc)	     
		     EXCEPT_WARNING,  // completed function but something may be wrong
		     EXCEPT_ERROR,    // could not complete function
		     EXCEPT_SEVERE //severe error, kill the system
};

//Macro to easily subclass exceptions
#define PARSE_EXCEPTION(_format)	\
({					\
	va_list args;			\
	va_start(args, _format);	\
	Parse(_format, args);		\
	va_end(args);			\
})

#define EXCEPT_STR_SIZE 2048

class OmException {

public:
	OmException(const string &name, OmExceptType type, const string &msg); 
	
	const string& GetName();
	const string& GetMessage();
	const string& GetType();

protected:
	void Parse(const char *format, va_list args);
	
	OmExceptType mType;
	string mName;
	string mMessage;
};

class OmAccessException : public OmException {
public:
	OmAccessException(string msg) 
	: OmException("OmAccessException", EXCEPT_WARNING, msg) { }
};

class OmFormatException : public OmException {
public:
	OmFormatException(string msg) 
	: OmException("OmFormatException", EXCEPT_WARNING, msg) { }
};

class OmIoException : public OmException {
public:
	OmIoException(string msg) 
	: OmException("OmIoException", EXCEPT_ERROR, msg) { }
};

class OmModificationException : public OmException {
public:
	OmModificationException(string msg) 
	: OmException("OmModificationException", EXCEPT_WARNING, msg) { }
};



#endif
