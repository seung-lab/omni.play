#pragma once

#include "common/common.h"
#include "actions/details/omUndoCommand.hpp"

class OmSegment;
class SegmentDataWrapper;
class SegmentationDataWrapper;
class OmSegmentValidateActionImpl;

class OmSegmentValidateAction : public OmUndoCommand {

public:
    OmSegmentValidateAction(boost::shared_ptr<OmSegmentValidateActionImpl> impl)
        : impl_(impl)
    {}

    OmSegmentValidateAction(const SegmentationDataWrapper& sdw,
                            boost::shared_ptr<std::set<OmSegment*> > selectedSegments,
                            const bool valid);

private:
    void Action();
    void UndoAction();
    std::string Description();
    void save(const std::string&);

    boost::shared_ptr<OmSegmentValidateActionImpl> impl_;
};

