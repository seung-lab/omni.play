#include "common/omDebug.h"
#include "actions/io/omActionLogger.hpp"
#include "actions/details/omSegmentSplitAction.h"
#include "actions/details/omSegmentSplitActionImpl.hpp"
#include "segment/omFindCommonEdge.hpp"

OmSegmentSplitAction::OmSegmentSplitAction( const SegmentationDataWrapper & sdw,
                                            const OmSegmentEdge & edge )
    : impl_(om::make_shared<OmSegmentSplitActionImpl>(sdw, edge))
{
    SetUndoable(true);
}

OmSegmentSplitAction::OmSegmentSplitAction( const SegmentDataWrapper & sdw,
                                            const DataCoord coord1, const DataCoord coord2)
    : impl_(om::make_shared<OmSegmentSplitActionImpl>(sdw, coord1, coord2))
{
    SetUndoable(true);
}

void OmSegmentSplitAction::Action()
{
    impl_->Execute();
}

void OmSegmentSplitAction::UndoAction()
{
    impl_->Undo();
}

std::string OmSegmentSplitAction::Description()
{
    return impl_->Description();
}

void OmSegmentSplitAction::save(const std::string& comment)
{
    OmActionLogger::save(impl_, comment);
}
