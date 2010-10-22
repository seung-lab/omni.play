#ifndef OM_MST_OLD_H
#define OM_MST_OLD_H

#include "datalayer/omDataWrapper.h"

class OmSegmentation;
class OmIDataReader;
class OmDataPath;

class OmMSTold {
public:
	OmMSTold();
	~OmMSTold(){}

	void readOld(OmSegmentation & seg);
	void import(const std::string& fname);

	OmDataWrapperPtr mDend;
	OmDataWrapperPtr mDendValues;
	OmDataWrapperPtr mEdgeDisabledByUser;
	OmDataWrapperPtr mEdgeForceJoin;

	int NumEdges() const {
		return mDendCount;
	}

private:
	bool importDend(OmIDataReader*);
	bool importDendValues(OmIDataReader*);
	void setupUserEdges();
	int mDendCount;

	OmDataPath getDendPath(OmSegmentation&);
	OmDataPath getDendValuesPath(OmSegmentation&);
	OmDataPath getEdgeDisabledByUserPath(OmSegmentation&);
	OmDataPath getEdgeForceJoinPath(OmSegmentation&);
};

#endif
