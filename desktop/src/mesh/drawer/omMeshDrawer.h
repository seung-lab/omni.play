#pragma once

#include "common/common.h"

class OmMeshPlanCache;
class OmSegmentation;
class OmViewGroupState;
class OmVolumeCuller;
class OmMeshSegmentList;
class OmMeshDrawerImpl;

class OmMeshDrawer {
private:
    OmSegmentation *const segmentation_;
    om::shared_ptr<OmMeshSegmentList> rootSegLists_;
    om::shared_ptr<OmMeshPlanCache> cache_;

    om::shared_ptr<OmVolumeCuller> culler_;
    int numPrevRedraws_;

public:
    OmMeshDrawer(OmSegmentation*);

    virtual ~OmMeshDrawer(){}

    boost::optional<std::pair<float,float> >
    Draw(OmViewGroupState*, om::shared_ptr<OmVolumeCuller>,
         const OmBitfield drawOptions);

private:
    void printDrawInfo(const OmMeshDrawerImpl& drawer);
    int getAllowedDrawTime();
    void updateNumPrevRedraws(om::shared_ptr<OmVolumeCuller> culler);
};

