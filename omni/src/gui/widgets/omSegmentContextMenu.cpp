#include "actions/omActions.hpp"
#include "common/omDebug.h"
#include "gui/inspectors/segObjectInspector.h"
#include "gui/widgets/omSegmentContextMenu.h"
#include "project/omProject.h"
#include "segment/omSegmentIterator.h"
#include "segment/omSegmentSelector.h"
#include "system/cache/omCacheManager.h"
#include "system/omEvents.h"
#include "system/omStateManager.h"
#include "utility/dataWrappers.h"
#include "viewGroup/omViewGroupState.h"

/////////////////////////////////
///////          Context Menu Methods

void OmSegmentContextMenu::Refresh(const SegmentDataWrapper& sdw,
								   OmViewGroupState* vgs)
{
	sdw_ = sdw;

	mViewGroupState = vgs;

	//clear old menu actions
	clear();

	addSelectionNames();
	addSeparator();

	addSelectionAction();
	addSeparator();

	addColorActions();
	addSeparator();

	if(!isValid()) {
		addDendActions();
		addSeparator();
	}

	addGroupActions();
	addSeparator();

	addPropertiesActions();
}

bool OmSegmentContextMenu::isValid() const {
	assert(sdw_.isValidWrapper());
	return sdw_.FindRoot()->IsValid();
}

bool OmSegmentContextMenu::isUncertain() const {
	assert(sdw_.isValidWrapper());
	return om::UNCERTAIN == sdw_.FindRoot()->GetListType();
}

void OmSegmentContextMenu::addSelectionNames()
{
	const QString segStr = QString("Segment %1 (Root %2)")
		.arg(sdw_.getID())
		.arg(sdw_.FindRootID());
	addAction(segStr);

	QString validStr;
	if(isValid()){
		validStr = QString("%1 %2")
			.arg("Valid in ")
			.arg(sdw_.getSegmentationName());
	} else {
		validStr = QString("%1 %2")
			.arg("Not valid in ")
			.arg(sdw_.getSegmentationName());
	}
	addAction(validStr);
}

/*
 *	adds Un/Select Segment Action
 */
void OmSegmentContextMenu::addSelectionAction()
{
	if(sdw_.isSelected()) {
		addAction("Select Only This Segment", this, SLOT(unselectOthers()));
		addAction("Deselect Only This Segment", this, SLOT(unselect()));
	} else {
		addAction("Select Only This Segment", this, SLOT(unselectOthers()));
		addAction("Select Segment", this, SLOT(select()));
	}
}

/*
 *	Merge Segments
 */
void OmSegmentContextMenu::addDendActions()
{
	addAction("Merge Selected Segments", this, SLOT(mergeSegments()));
	addAction("Split Segments", this, SLOT(splitSegments()));
}


/////////////////////////////////
///////          Context Menu Slots Methods

void OmSegmentContextMenu::select()
{
	OmSegmentSelector sel(sdw_.getSegmentationID(), this, "view3d" );
	sel.augmentSelectedSet(sdw_.getID(), true);
	sel.sendEvent();
}

void OmSegmentContextMenu::unselect()
{
	OmSegmentSelector sel(sdw_.getSegmentationID(), this, "view3d" );
	sel.augmentSelectedSet(sdw_.getID(), false);
	sel.sendEvent();
}

void OmSegmentContextMenu::unselectOthers()
{
	OmSegmentSelector sel(sdw_.getSegmentationID(), this, "view3d" );
	sel.selectNoSegments();
	sel.selectJustThisSegment(sdw_.getID(), true);
	sel.sendEvent();
}

void OmSegmentContextMenu::mergeSegments()
{
	OmSegIDsSet ids = sdw_.getSegmentCache()->GetSelectedSegmentIds();
	OmActions::JoinSegments(sdw_.getSegmentationID(), ids);
}

void OmSegmentContextMenu::splitSegments()
{
	mViewGroupState->SetShowSplitMode(true);
	OmStateManager::SetToolModeAndSendEvent(SPLIT_MODE);
	mViewGroupState->SetSplitMode(sdw_);
}

void OmSegmentContextMenu::addColorActions()
{
	addAction("Randomize Segment Color", this, SLOT(randomizeColor()));
}

void OmSegmentContextMenu::addGroupActions()
{
	addAction("Set Segment Valid", this, SLOT(setValid()));
	addAction("Set Segment Not Valid", this, SLOT(setNotValid()));
}

void OmSegmentContextMenu::randomizeColor()
{
	OmSegment* segment = sdw_.FindRoot();
	segment->reRandomizeColor();

	OmCacheManager::TouchFresheness();
	OmEvents::Redraw2d();
}

void OmSegmentContextMenu::setValid()
{
	//debug(validate, "OmSegmentContextMenu::addGroup\n");
	if(sdw_.isValidWrapper()){
		OmActions::ValidateSegment(sdw_, om::SET_VALID);
		OmEvents::SegmentModified();
	}
}

void OmSegmentContextMenu::setNotValid()
{
	//debug(validate, "OmSegmentContextMenu::addGroup\n");
	if(sdw_.isValidWrapper()){
		OmActions::ValidateSegment(sdw_, om::SET_NOT_VALID);
		OmEvents::SegmentModified();
	}
}

void OmSegmentContextMenu::showProperties()
{
	const OmSegID segid = sdw_.FindRootID();

	mViewGroupState->GetInspectorProperties()->
		setOrReplaceWidget( new SegObjectInspector(sdw_, this),
							QString("Segmentation %1: Segment %2")
							.arg(sdw_.getSegmentationID())
							.arg(segid));
}

void OmSegmentContextMenu::addPropertiesActions()
{
	addAction("Properties", this, SLOT(showProperties()));
	addAction("List Children", this, SLOT(printChildren()));
}

void OmSegmentContextMenu::printChildren()
{
	//debug(validate, "OmSegmentContextMenu::addGroup\n");
	if (sdw_.isValidWrapper()){
		OmSegmentCache* segCache = sdw_.getSegmentCache();
		OmSegmentIterator iter(segCache);
		iter.iterOverSegmentID(sdw_.FindRootID());

		OmSegment * seg = iter.getNextSegment();
		while(NULL != seg) {
			const QString str = QString("%1 : %2, %3, %4")
				.arg(seg->value())
				.arg(seg->getParentSegID())
				.arg(seg->getThreshold())
				.arg(seg->size());
			printf("%s\n", qPrintable(str));
			seg = iter.getNextSegment();
		}
	}
}
