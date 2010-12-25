#ifndef OM_DATA_LAYER_H
#define OM_DATA_LAYER_H

#include "common/omCommon.h"
#include "common/om.hpp"

class OmIDataReader;
class OmIDataWriter;

class OmDataLayer {
public:
	static OmIDataReader* getReader(const std::string& fileNameAndPath,
					const bool readOnly, const om::Affinity = om::NO_AFFINITY);
	static OmIDataWriter* getWriter(const std::string& fileNameAndPath,
					const bool readOnly);
private:
	OmDataLayer(){}
	~OmDataLayer(){}
};

#endif
