#ifndef OM_DATA_LAYER_H
#define OM_DATA_LAYER_H

#include "utility/omDataReader.h"
#include "utility/omDataWriter.h"

class OmDataLayer
{
 public:
	OmDataLayer();
	~OmDataLayer();
	OmDataReader * getReader( QString fileNameAndPath, const bool autoOpenAndClose );
	OmDataWriter * getWriter( QString fileNameAndPath, const bool autoOpenAndClose );
};

#endif
