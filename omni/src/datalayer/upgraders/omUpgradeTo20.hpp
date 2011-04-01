#ifndef OM_UPGRADE_TO_20_HPP
#define OM_UPGRADE_TO_20_HPP

#include "segment/omSegmentUtils.hpp"
#include "utility/dataWrappers.h"

class OmUpgradeTo20{
public:
    void GroupUpValidSegments(){
        SegmentationDataWrapper sdw(1);
        groupUpValidSegments(sdw);
    }

private:
    void groupUpValidSegments(const SegmentationDataWrapper& sdw)
    {
        printf("rewriting valid segment data...\n");
        OmSegmentUtils::ReValidateEveryObject(sdw);
    }
};

#endif
