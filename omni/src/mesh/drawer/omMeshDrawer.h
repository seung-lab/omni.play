#ifndef OM_MESH_DRAWER_H
#define OM_MESH_DRAWER_H

#include "common/omCommon.h"

class OmMeshPlanCache;
class OmSegmentation;
class OmViewGroupState;
class OmVolumeCuller;
class OmMeshSegmentList;
class OmMeshDrawerImpl;

class OmMeshDrawer {
private:
	OmSegmentation *const segmentation_;
	boost::shared_ptr<OmMeshSegmentList> rootSegLists_;
	boost::shared_ptr<OmMeshPlanCache> cache_;

	boost::shared_ptr<OmVolumeCuller> culler_;
	int numPrevRedraws_;

public:
	OmMeshDrawer(OmSegmentation*);

	virtual ~OmMeshDrawer(){}

	boost::optional<std::pair<float,float> >
	Draw(OmViewGroupState*, boost::shared_ptr<OmVolumeCuller>,
			  const OmBitfield drawOptions);

private:
	void printDrawInfo(const OmMeshDrawerImpl& drawer);
	int getAllowedDrawTime();
	void updateNumPrevRedraws(boost::shared_ptr<OmVolumeCuller> culler);
};

#endif
