#ifndef OM_DATA_LAYER_H
#define OM_DATA_LAYER_H

#include "common/omCommon.h"

class OmIDataReader;
class OmDataWriter;
class OmDataPath;

class OmDataLayer {
public:
	static OmIDataReader* getReader(const QString& fileNameAndPath,
					const bool readOnly);
	static OmDataWriter* getWriter(const QString& fileNameAndPath,
				       const bool readOnly);
private:
	OmDataLayer(){}
	~OmDataLayer(){}
};

#endif
