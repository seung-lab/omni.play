#pragma once

#include "actions/details/omUndoCommand.hpp"
#include "common/common.h"

class OmSegment;
class SegmentDataWrapper;
class SegmentationDataWrapper;
class OmSegmentUncertainActionImpl;

class OmSegmentUncertainAction : public OmUndoCommand {

public:
    OmSegmentUncertainAction(boost::shared_ptr<OmSegmentUncertainActionImpl> impl)
        : impl_(impl)
    {}

    OmSegmentUncertainAction(const SegmentationDataWrapper& sdw,
                             boost::shared_ptr<std::set<OmSegment*> > selectedSegments,
                             const bool valid);
private:
    void Action();
    void UndoAction();
    std::string Description();
    void save(const std::string&);

    boost::shared_ptr<OmSegmentUncertainActionImpl> impl_;

};

