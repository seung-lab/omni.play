#ifndef OM_HDF_DATASET_H
#define OM_HDF_DATASET_H

#include <string>
using std::string;

class OmHdf5DataSet {
public:
	OmHdf5DataSet (string inname, int insize, const void* indata);

	const string name;
	const int size;
	const void* data;
};


#endif
