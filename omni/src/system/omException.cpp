
#include "omException.h"
#include "system/omDebug.h"

string OmExceptTypeStrings[] = { "Note",
	"Information",
	"Warning",
	"Error",
	"Critical Error"
};

/*
 * Utility Methods
 */
string exceptTypeToString(OmExceptType t);
string execptString(const char *format, ...);

OmException::OmException(const string & name, OmExceptType type, const string & msg)
:mName(name), mType(type), mMessage(msg)
{

	Log();
}

const string & OmException::GetName()
{
	return mName;
}

const string & OmException::GetMessage()
{
	return mMessage;
}

const string & OmException::GetType()
{
	return OmExceptTypeStrings[mType];
}

void OmException::Parse(const char *format, va_list args)
{

	//parse message
	char error_buf[EXCEPT_STR_SIZE];
	vsnprintf(error_buf, sizeof(error_buf), format, args);
	mMessage.assign(error_buf);

	//Log
	Log();
}

void OmException::Log()
{
	cout << mName << " " << exceptTypeToString(mType) << ": " << mMessage << endl;
}

string exceptTypeToString(OmExceptType t)
{
	return OmExceptTypeStrings[t];
}

string execptString(const char *format, ...)
{
	char error_buf[EXCEPT_STR_SIZE];

	va_list args;
	va_start(args, format);
	vsnprintf(error_buf, sizeof(error_buf), format, args);
	va_end(args);

	return string(error_buf);
}
