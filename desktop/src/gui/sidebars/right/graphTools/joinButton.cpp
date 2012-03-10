#include "actions/omActions.h"
#include "common/omDebug.h"
#include "gui/sidebars/right/graphTools/graphTools.h"
#include "gui/sidebars/right/graphTools/joinButton.h"
#include "segment/omSegments.h"
#include "utility/dataWrappers.h"
#include "volume/omSegmentation.h"

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

    OmSegmentation& seg = sdw.GetSegmentation();
    const OmSegIDsSet ids = seg.Segments()->GetSelectedSegmentIDs();
    OmActions::JoinSegments(sdw, ids);

    mParent->updateGui();
}
