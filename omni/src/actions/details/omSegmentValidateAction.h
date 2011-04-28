#ifndef OM_SEGMENT_VALIDATE_ACTION_H
#define OM_SEGMENT_VALIDATE_ACTION_H

#include "common/om.hpp"
#include "common/omCommon.h"
#include "actions/details/omUndoCommand.hpp"

class OmSegment;
class SegmentDataWrapper;
class SegmentationDataWrapper;
class OmSegmentValidateActionImpl;

class OmSegmentValidateAction : public OmUndoCommand {

public:
    OmSegmentValidateAction(om::shared_ptr<OmSegmentValidateActionImpl> impl)
        : impl_(impl)
    {}

    OmSegmentValidateAction(const SegmentationDataWrapper& sdw,
                            om::shared_ptr<std::set<OmSegment*> > selectedSegments,
                            const bool valid);

private:
    void Action();
    void UndoAction();
    std::string Description();
    void save(const std::string&);

    om::shared_ptr<OmSegmentValidateActionImpl> impl_;
};

#endif
