#pragma once

#include "actions/details/omUndoCommand.hpp"
#include "segment/omSegmentEdge.h"

class SegmentDataWrapper;
class SegmentDataWrapper;
class OmSegmentation;
class OmSegment;
class OmViewGroupState;
class OmSegmentShatterActionImpl;

class OmSegmentShatterAction : public OmUndoCommand {

public:
    OmSegmentShatterAction(boost::shared_ptr<OmSegmentShatterActionImpl> impl)
        : impl_(impl)
    {}

    OmSegmentShatterAction(const SegmentDataWrapper& sdw);

private:
    void Action();
    void UndoAction();
    std::string Description();
    void save(const std::string& comment);

    boost::shared_ptr<OmSegmentShatterActionImpl> impl_;
};

