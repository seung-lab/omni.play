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
#include <QString>

enum OmExceptType {  EXCEPT_NOTE = 0, // not reported to user, make internal note
		     EXCEPT_INFO,     // give user information (usage, formatting, etc)	     
		     EXCEPT_WARNING,  // completed function but something may be wrong
		     EXCEPT_ERROR,    // could not complete function
		     EXCEPT_SEVERE    //severe error, kill the system
};

class OmException {

public:
	OmException(const string &name, OmExceptType type, const string &msg); 
	
	QString GetName();
	QString GetMessage();
	QString GetType();

protected:
	string mName;
	OmExceptType mType;
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
