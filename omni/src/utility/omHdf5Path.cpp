#include "omHdf5Path.h"

OmHdf5Path::OmHdf5Path()
{
}

void OmHdf5Path::setPath( std::string str )
{
	mPath = str;
}

void OmHdf5Path::setPathQstr( QString str )
{
        mPath = str.toStdString();
}

const std::string & OmHdf5Path::getString() const
{
	return mPath;
}

OmHdf5Path& OmHdf5Path::operator=( OmHdf5Path & rhs ) {
	this->mPath = rhs.mPath;
	return *this;
}
