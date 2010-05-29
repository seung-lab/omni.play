#ifndef OM_HDF_HELPERS_H
#define OM_HDF_HELPERS_H

#include "omDataPath.h"

class OmHdf5Helpers 
{
 public:
	static OmDataPath getDefaultDatasetName();
	static OmDataPath getProjectArchiveNameQT();
};

#endif
