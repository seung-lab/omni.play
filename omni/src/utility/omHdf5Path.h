#ifndef OM_HDF_PATH_H
#define OM_HDF_PATH_H

#include <QString>
#include <string>
using namespace std;

class OmHdf5Path 
{
 public:
	OmHdf5Path();
	void setPath( string str );
	string getString();

 private:
	QString mPath;
}; 

#endif
