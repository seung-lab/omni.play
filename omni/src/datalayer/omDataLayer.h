#ifndef OM_DATA_LAYER_H
#define OM_DATA_LAYER_H

#include "common/omCommon.h"
#include "datalayer/omDataReader.h"
#include "datalayer/omDataWriter.h"

class OmDataLayer
{
 public:
	OmDataLayer();
	~OmDataLayer();
	OmDataReader * getReader( QString fileNameAndPath, 
				  const bool autoOpenAndClose, const bool readOnly );
	OmDataWriter * getWriter( QString fileNameAndPath, 
				  const bool autoOpenAndClose, const bool readOnly );
};

#endif
