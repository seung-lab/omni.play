#include "actions/omActions.h"
#include "gui/sidebars/right/groupButtonTag.h"
#include "common/omDebug.h"
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
                                          seg.Segments()->GetSelectedSegmentIds(),
                                          mParent->getGroupNameFromGUI(),
                                          true);

    OmEvents::SegmentModified();
}
