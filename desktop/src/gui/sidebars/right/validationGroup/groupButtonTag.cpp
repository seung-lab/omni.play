#include "actions/omActions.h"
#include "gui/sidebars/right/validationGroup/groupButtonTag.h"
#include "common/logging.h"
#include "utility/dataWrappers.h"
#include "volume/omSegmentation.h"
#include "events/omEvents.h"
#include "segment/omSegments.h"

GroupButtonTag::GroupButtonTag(ValidationGroup * d)
    : OmButton<ValidationGroup>( d,
                                 "Group As:",
                                 "",
                                 false)
{}

void GroupButtonTag::doAction()
{
    //debug(dendbar, "ValidationGroup::specialGroupAdd\n");

    SegmentationDataWrapper sdw = mParent->GetSDW();

    if(!sdw.IsSegmentationValid()){
        return;
    }

    OmSegmentation & seg = sdw.GetSegmentation();

    OmActions::CreateOrDeleteSegmentGroup(seg.GetID(),
                                          seg.Segments()->GetSelectedSegmentIDs(),
                                          mParent->getGroupNameFromGUI().toStdString(),
                                          true);

    OmEvents::SegmentModified();
}
