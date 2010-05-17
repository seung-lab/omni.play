#ifndef OM_HDF_PATH_H
#define OM_HDF_PATH_H

#include "common/omCommon.h"
#include <string>

class OmHdf5Path 
{
 public:
	OmHdf5Path();
	void setPath( std::string str );
	void setPathQstr( QString );

	std::string getString();
	QString getStringQStr();

	OmHdf5Path & operator=( OmHdf5Path & rhs );

 private:
	QString mPath;
}; 

#endif
