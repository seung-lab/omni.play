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
    boost::shared_ptr<OmMeshSegmentList> rootSegLists_;
    boost::shared_ptr<OmMeshPlanCache> cache_;

    boost::shared_ptr<OmVolumeCuller> culler_;
    int numPrevRedraws_;

public:
    OmMeshDrawer(OmSegmentation*);

    virtual ~OmMeshDrawer(){}

    boost::optional<std::pair<float,float> >
    Draw(OmViewGroupState*, boost::shared_ptr<OmVolumeCuller>,
         const om::common::Bitfield drawOptions);

private:
    void printDrawInfo(const OmMeshDrawerImpl& drawer);
    int getAllowedDrawTime();
    void updateNumPrevRedraws(boost::shared_ptr<OmVolumeCuller> culler);
};

