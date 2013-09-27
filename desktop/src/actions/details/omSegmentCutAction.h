#pragma once

#include "actions/details/omUndoCommand.hpp"
#include "segment/omSegmentEdge.h"

class SegmentDataWrapper;
class SegmentationDataWrapper;
class OmSegmentation;
class OmSegment;
class OmViewGroupState;
class OmSegmentCutActionImpl;

class OmSegmentCutAction : public OmUndoCommand {

public:
    OmSegmentCutAction(std::shared_ptr<OmSegmentCutActionImpl> impl)
        : impl_(impl)
    {}

    OmSegmentCutAction(const SegmentDataWrapper& sdw);

private:
    void Action();
    void UndoAction();
    std::string Description();
    void save(const std::string& comment);

    std::shared_ptr<OmSegmentCutActionImpl> impl_;
};

