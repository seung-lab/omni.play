#ifndef OM_DATA_PATHS_H
#define OM_DATA_PATHS_H

#include "utility/omHdf5Path.h"

class OmDataPaths
{
 public:
	static OmHdf5Path getSegmentPagePath( OmId segmentationID, const quint32 pageNum );

};

#endif
