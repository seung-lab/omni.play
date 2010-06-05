#ifndef OM_DATA_PATH_H
#define OM_DATA_PATH_H

#include "common/omCommon.h"

class OmDataPath 
{
 public:
 	OmDataPath();
 	OmDataPath( string s ) : mPath(s) {}

	void setPath( std::string str );
	void setPathQstr( QString );

	const std::string & getString() const;

	OmDataPath & operator=( OmDataPath & rhs );

 private:
	std::string mPath;
}; 

#endif
