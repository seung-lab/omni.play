#include "omHdf5Path.h"

OmHdf5Path::OmHdf5Path()
{
}

void OmHdf5Path::setPath( string str )
{
	mPath = QString::fromStdString(str);
}

string OmHdf5Path::getString()
{
	return mPath.toStdString();
}
