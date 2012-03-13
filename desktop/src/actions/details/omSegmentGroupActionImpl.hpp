#pragma once

#include "common/omCommon.h"
#include "common/omString.hpp"
#include "system/omGroups.h"
#include "utility/omCopyFirstN.hpp"
#include "utility/dataWrappers.h"

class OmSegmentGroupActionImpl {
private:
    OmID mSegmentationId;
    OmGroupName mName;
    bool mCreate;
    OmSegIDsSet mSelectedSegmentIds;

public:
    OmSegmentGroupActionImpl()
    {}

    OmSegmentGroupActionImpl(const OmID segmentationId,
                             const OmSegIDsSet& selectedSegmentIds,
                             const OmGroupName name,
                             const bool create)
        : mSegmentationId(segmentationId)
        , mName(name)
        , mCreate(create)
        , mSelectedSegmentIds(selectedSegmentIds)
    {}

    void Execute()
    {
        SegmentationDataWrapper sdw(mSegmentationId);
        OmSegmentation& seg = sdw.GetSegmentation();

        if(mCreate) {
            seg.Groups()->SetGroup(mSelectedSegmentIds, mName);
        } else {
            seg.Groups()->UnsetGroup(mSelectedSegmentIds, mName);
        }
    }

    void Undo()
    {
        SegmentationDataWrapper sdw(mSegmentationId);
        OmSegmentation& seg = sdw.GetSegmentation();

        if(!mCreate) {
            seg.Groups()->SetGroup(mSelectedSegmentIds, mName);
        } else {
            seg.Groups()->UnsetGroup(mSelectedSegmentIds, mName);
        }
    }

    std::string Description() const
    {
        static const int max = 5;

        const std::string nums =
            om::utils::MakeShortStrList<OmSegIDsSet, OmSegID>(mSelectedSegmentIds, max);

        const std::string prefix("Grouped: ");

        return prefix + nums;
    }

    QString classNameForLogFile() const {
        return "OmSegmentGroupAction";
    }

private:
    template <typename T> friend class OmActionLoggerThread;
    friend class QDataStream &operator<<(QDataStream&, const OmSegmentGroupActionImpl&);
    friend class QDataStream &operator>>(QDataStream&,  OmSegmentGroupActionImpl&);
};

