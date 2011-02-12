#include "segment/omSegmentUtils.hpp"
#include "actions/io/omActionLogger.hpp"
#include "actions/details/omSegmentUncertainAction.h"
#include "actions/details/omSegmentUncertainActionImpl.hpp"

void OmSegmentUncertainAction::SetUncertain(const SegmentDataWrapper& sdw,
											const bool uncertain)
{
	OmSegIDsSet set;
	set.insert(sdw.FindRootID());

	boost::shared_ptr<std::set<OmSegment*> > children =
		OmSegmentUtils::GetAllChildrenSegments(sdw.SegmentCache(), set);

	(new OmSegmentUncertainAction(sdw.MakeSegmentationDataWrapper(),
								  children, uncertain))->Run();
}

void OmSegmentUncertainAction::SetUncertain(const SegmentationDataWrapper& sdw,
											const bool uncertain)
{
	OmSegmentCache* segCache = sdw.SegmentCache();
	boost::shared_ptr<std::set<OmSegment*> > children =
		OmSegmentUtils::GetAllChildrenSegments(segCache,
											   segCache->GetSelectedSegmentIds());

	(new OmSegmentUncertainAction(sdw, children, uncertain))->Run();
}

OmSegmentUncertainAction::OmSegmentUncertainAction(const SegmentationDataWrapper& sdw,
												   boost::shared_ptr<std::set<OmSegment*> > selectedSegments,
												   const bool uncertain)
	: impl_(boost::make_shared<OmSegmentUncertainActionImpl>(sdw,
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
	OmActionLogger::save(impl_, comment);
}
