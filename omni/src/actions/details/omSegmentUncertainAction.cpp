#include "segment/omSegmentUtils.hpp"
#include "actions/io/omActionLoggerFS.h"
#include "actions/details/omSegmentUncertainAction.h"
#include "actions/details/omSegmentUncertainActionImpl.hpp"

void OmSegmentUncertainAction::SetUncertain(const SegmentDataWrapper& sdw,
											const bool uncertain)
{
	OmSegIDsSet set;
	set.insert(sdw.FindRootID());

	boost::shared_ptr<std::set<OmSegment*> > children =
		OmSegmentUtils::GetAllChildrenSegments(sdw.GetSegmentCache(), set);

	(new OmSegmentUncertainAction(sdw.getID(), children, uncertain))->Run();
}

void OmSegmentUncertainAction::SetUncertain(const SegmentationDataWrapper& sdw,
											const bool uncertain)
{
	OmSegmentCache* segCache = sdw.GetSegmentCache();
	boost::shared_ptr<std::set<OmSegment*> > children =
		OmSegmentUtils::GetAllChildrenSegments(segCache,
											   segCache->GetSelectedSegmentIds());

	(new OmSegmentUncertainAction(sdw.getID(), children, uncertain))->Run();
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
