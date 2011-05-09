#include "common/omDebug.h"
#include "gui/sidebars/right/joinButton.h"
#include "volume/omSegmentation.h"
#include "utility/dataWrappers.h"
#include "segment/omSegments.h"
#include "actions/omActions.h"
#include "gui/sidebars/right/graphTools.h"

JoinButton::JoinButton(GraphTools * d)
    : OmButton<GraphTools>( d,
                            "Join",
                            "Join objects",
                            false)
{
}

void JoinButton::doAction()
{
    SegmentationDataWrapper sdw = mParent->GetSDW();
    if(!sdw.IsSegmentationValid()){
        return;
    }

    OmSegmentation & seg = sdw.GetSegmentation();
    const OmSegIDsSet ids = seg.Segments()->GetSelectedSegmentIds();
    OmActions::JoinSegments(sdw.GetID(), ids);

    mParent->updateGui();
}
