#ifndef OM_DATA_PATHS_H
#define OM_DATA_PATHS_H

#include "utility/omHdf5Path.h"

class OmDataPaths
{
 public:
	static OmHdf5Path getSegmentValuePath( OmId segmentationID, SEGMENT_DATA_TYPE value );
	static OmHdf5Path getSegmentPath(OmId segmentationID, OmId segmentID );

};

#endif
