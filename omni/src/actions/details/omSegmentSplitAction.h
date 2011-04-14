#ifndef OM_SEGMENT_SPLIT_ACTION_H
#define OM_SEGMENT_SPLIT_ACTION_H

#include "actions/details/omUndoCommand.hpp"
#include "segment/omSegmentEdge.h"

class SegmentDataWrapper;
class SegmentationDataWrapper;
class OmSegmentation;
class OmSegment;
class OmViewGroupState;
class OmSegmentSplitActionImpl;

class OmSegmentSplitAction : public OmUndoCommand {

public:
    OmSegmentSplitAction(om::shared_ptr<OmSegmentSplitActionImpl> impl)
        : impl_(impl)
    {}

    OmSegmentSplitAction(const SegmentationDataWrapper& sdw,
                          const OmSegmentEdge& edge);
    OmSegmentSplitAction(const SegmentDataWrapper & sdw,
                         const DataCoord coord1, const DataCoord coord2);

private:
    void Action();
    void UndoAction();
    std::string Description();
    void save(const std::string& comment);

    om::shared_ptr<OmSegmentSplitActionImpl> impl_;
};

#endif
