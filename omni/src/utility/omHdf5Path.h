#ifndef OM_HDF_PATH_H
#define OM_HDF_PATH_H

#include <QString>
#include <string>

class OmHdf5Path 
{
 public:
	OmHdf5Path();
	void setPath( std::string str );
	std::string getString();

	OmHdf5Path operator=( OmHdf5Path & rhs ) {
		this->mPath = rhs.mPath;
	}

 private:
	QString mPath;
}; 

#endif
