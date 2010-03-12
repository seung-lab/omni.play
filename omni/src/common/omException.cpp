#include "omException.h"
#include "common/omDebug.h"

static string OmExceptTypeStrings[] = { "Note",
					"Information",
					"Warning",
					"Error",
					"Critical Error"
};

OmException::OmException(const string & name, OmExceptType type, const string & msg)
:mName(name), mType(type), mMessage(msg)
{
}

QString OmException::GetName()
{
	return QString::fromStdString( mName );
}

QString OmException::GetMessage()
{
	return QString::fromStdString( mMessage );
}

QString OmException::GetType()
{
	return QString::fromStdString( OmExceptTypeStrings[mType] );
}
