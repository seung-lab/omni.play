#pragma once

#include "common/omCommon.h"
#include "events/omEvents.h"
#include "project/omProject.h"
#include "segment/lists/omSegmentLists.h"
#include "system/cache/omCacheManager.h"
#include "utility/dataWrappers.h"
#include "viewGroup/omViewGroupState.h"

class OmSegmentSplitActionImpl {
private:
    OmSegmentEdge mEdge;
    OmID mSegmentationID;
    OmSegID mSegID;
    DataCoord mCoord1;
    DataCoord mCoord2;
    QString desc;

public:
    OmSegmentSplitActionImpl() {}
    OmSegmentSplitActionImpl(const SegmentationDataWrapper& sdw,
                             const OmSegmentEdge& edge)
        : mEdge(edge)
        , mSegmentationID(sdw.GetSegmentationID())
        , mSegID(0)
        , mCoord1()
        , mCoord2()
        , desc("Splitting: ")
    {}

    OmSegmentSplitActionImpl(const SegmentDataWrapper& sdw,
                             const DataCoord& coord1,
                             const DataCoord& coord2)
        : mEdge(0,0,0.0)
        , mSegmentationID(sdw.GetSegmentationID())
        , mSegID(sdw.GetSegmentID())
        , mCoord1(coord1)
        , mCoord2(coord2)
        , desc("Splitting: ")
    {}

    void Execute()
    {
        SegmentationDataWrapper sdw(mSegmentationID);

        if(!mSegID)
        {
            mEdge = sdw.Segments()->SplitEdge(mEdge);

            desc = QString("Split seg %1 from %2")
                .arg(mEdge.childID)
                .arg(mEdge.parentID);

            std::cout << desc.toStdString() << "\n";

            OmEvents::SegmentModified();

            sdw.SegmentLists()->RefreshGUIlists();

        } else {
            mEdge = sdw.Segments()->SplitSegment(mSegID, mCoord1, mCoord2);
        }

        OmEvents::Redraw2d();
        OmEvents::Redraw3d();
        OmCacheManager::TouchFreshness();
    }

    void Undo()
    {
        SegmentationDataWrapper sdw(mSegmentationID);
        if(!mSegID)
        {
            std::pair<bool, OmSegmentEdge> edge = sdw.Segments()->JoinEdge(mEdge);

            if(!mEdge.childID || !mEdge.parentID) {
                printf("Can't undo a join that probably failed.\n");
                return;
            }

            assert(edge.first && "edge could not be rejoined...");
            mEdge = edge.second;

            desc = QString("Joined seg %1 to %2")
                .arg(mEdge.childID)
                .arg(mEdge.parentID);

            OmEvents::SegmentModified();

            sdw.SegmentLists()->RefreshGUIlists();

        } else {
            sdw.Segments()->UnSplitSegment(mEdge);
        }

        OmEvents::Redraw2d();
        OmEvents::Redraw3d();
        OmCacheManager::TouchFreshness();
    }

    std::string Description() const {
        return desc.toStdString();
    }

    QString classNameForLogFile() const {
        return "OmSegmentSplitAction";
    }

private:
    template <typename T> friend class OmActionLoggerThread;
    friend class QDataStream &operator<<(QDataStream&, const OmSegmentSplitActionImpl&);
    friend class QDataStream &operator>>(QDataStream&, OmSegmentSplitActionImpl&);
    friend QTextStream& operator<<(QTextStream& out, const OmSegmentSplitActionImpl&);
};

