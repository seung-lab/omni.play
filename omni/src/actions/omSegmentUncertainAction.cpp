#include "datalayer/fs/omActionLoggerFS.h"
#include "actions/omSegmentUncertainAction.h"
#include "actions/omSegmentUncertainActionImpl.hpp"

void OmSegmentUncertainAction::SetUncertain(const SegmentDataWrapper& sdw,
											const bool uncertain)
{
	OmSegIDsSet set;
	set.insert(sdw.FindRootID());

	OmSegmentation & seg = sdw.getSegmentation();
	boost::shared_ptr<std::set<OmSegment*> > children =
		seg.GetAllChildrenSegments(set);

	(new OmSegmentUncertainAction(seg.GetID(), children, uncertain))->Run();
}

void OmSegmentUncertainAction::SetUncertain(const SegmentationDataWrapper& sdw,
											const bool uncertain)
{
	OmSegmentation & seg = sdw.getSegmentation();

	boost::shared_ptr<std::set<OmSegment*> > children =
		seg.GetAllChildrenSegments(seg.GetSegmentCache()->GetSelectedSegmentIds());

	(new OmSegmentUncertainAction(seg.GetID(), children, uncertain))->Run();
}

OmSegmentUncertainAction::OmSegmentUncertainAction(const OmID segmentationId,
												   boost::shared_ptr<std::set<OmSegment*> > selectedSegments,
												   const bool uncertain)
	: impl_(boost::make_shared<OmSegmentUncertainActionImpl>(segmentationId,
															 selectedSegments,
															 uncertain))
{
	SetUndoable(true);
}

/////////////////////////////////
///////          Action Methods
void OmSegmentUncertainAction::Action()
{
	impl_->Execute();
}

void OmSegmentUncertainAction::UndoAction()
{
	impl_->Undo();
}

std::string OmSegmentUncertainAction::Description()
{
	return impl_->Description();
}

void OmSegmentUncertainAction::save(const std::string& comment)
{
	OmActionLoggerFS::save(impl_, comment);
}
