#include "omException.h"
#include "common/omDebug.h"

static std::string OmExceptTypeStrings[] = { "Note",
					"Information",
					"Warning",
					"Error",
					"Critical Error"
};

OmException::OmException(const std::string & name, OmExceptType type,
						 const std::string & msg)
	: mName(name)
	, mType(type)
	, mMessage(msg)
{
	printf("\nOmException: %s\n\n", msg.c_str() );
}

QString OmException::GetName()
{
	return QString::fromStdString( mName );
}

QString OmException::GetMsg()
{
	return QString::fromStdString( mMessage );
}

QString OmException::GetType()
{
	return QString::fromStdString( OmExceptTypeStrings[mType] );
}
