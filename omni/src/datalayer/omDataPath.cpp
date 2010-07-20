#include "omDataPath.h"

OmDataPath::OmDataPath()
{
}

void OmDataPath::setPath( std::string str )
{
	mPath = str;
}

void OmDataPath::setPathQstr( QString str )
{
        mPath = str.toStdString();
}

const std::string & OmDataPath::getString() const
{
	return mPath;
}

OmDataPath& OmDataPath::operator=( OmDataPath & rhs ) {
	this->mPath = rhs.mPath;
	return *this;
}
