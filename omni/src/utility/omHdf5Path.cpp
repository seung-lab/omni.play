#include "omHdf5Path.h"

OmHdf5Path::OmHdf5Path()
{
}

void OmHdf5Path::setPath( std::string str )
{
	mPath = QString::fromStdString(str);
}

std::string OmHdf5Path::getString()
{
	return mPath.toStdString();
}
