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

	const std::string & getString() const;

	OmHdf5Path & operator=( OmHdf5Path & rhs );

 private:
	string mPath;
}; 

#endif
