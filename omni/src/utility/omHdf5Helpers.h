#ifndef OM_HDF_HELPERS_H
#define OM_HDF_HELPERS_H

#include "omHdf5Path.h"

class OmHdf5Helpers 
{
 public:
	static OmHdf5Path getDefaultDatasetName();
	static OmHdf5Path getProjectArchiveNameQT();
};

#endif
