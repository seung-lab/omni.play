#include "gui/inspectors/inspectorProperties.h"
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
#include "system/omGroup.h"
#include "system/omGroups.h"

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

	addDisableAction();
	addPropertiesActions();

	addSeparator();
	addGroups();
}

bool OmSegmentContextMenu::isValid() const {
	assert(sdw_.IsValidSegment());
	return sdw_.FindRoot()->IsValidListType();
}

bool OmSegmentContextMenu::isUncertain() const {
	assert(sdw_.IsValidSegment());
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
			.arg(sdw_.GetSegmentationName());
	} else {
		validStr = QString("%1 %2")
			.arg("Not valid in ")
			.arg(sdw_.GetSegmentationName());
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
	OmSegmentSelector sel(sdw_.MakeSegmentationDataWrapper(), this, "view3d" );
	sel.augmentSelectedSet(sdw_.getID(), true);
	sel.sendEvent();
}

void OmSegmentContextMenu::unselect()
{
	OmSegmentSelector sel(sdw_.MakeSegmentationDataWrapper(), this, "view3d" );
	sel.augmentSelectedSet(sdw_.getID(), false);
	sel.sendEvent();
}

void OmSegmentContextMenu::unselectOthers()
{
	OmSegmentSelector sel(sdw_.MakeSegmentationDataWrapper(), this, "view3d" );
	sel.selectNoSegments();
	sel.selectJustThisSegment(sdw_.getID(), true);
	sel.sendEvent();
}

void OmSegmentContextMenu::mergeSegments()
{
	OmSegIDsSet ids = sdw_.GetSegmentCache()->GetSelectedSegmentIds();
	OmActions::JoinSegments(sdw_.GetSegmentationID(), ids);
}

void OmSegmentContextMenu::splitSegments()
{
	mViewGroupState->SetShowSplitMode(true);
	OmStateManager::SetToolModeAndSendEvent(SPLIT_MODE);
	mViewGroupState->SetSplitMode(sdw_);
}

void OmSegmentContextMenu::addColorActions()
{
	addAction("Randomize Root Segment Color", this, SLOT(randomizeColor()));
	addAction("Randomize Segment Color", this, SLOT(randomizeSegmentColor()));
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

void OmSegmentContextMenu::randomizeSegmentColor()
{
        OmSegment* segment = sdw_.getSegment();
        segment->reRandomizeColor();

        OmCacheManager::TouchFresheness();
        OmEvents::Redraw2d();
}

void OmSegmentContextMenu::setValid()
{
	//debug(validate, "OmSegmentContextMenu::addGroup\n");
	if(sdw_.IsValidSegment()){
		OmActions::ValidateSegment(sdw_, om::SET_VALID);
		OmEvents::SegmentModified();
	}
}

void OmSegmentContextMenu::setNotValid()
{
	//debug(validate, "OmSegmentContextMenu::addGroup\n");
	if(sdw_.IsValidSegment()){
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
							.arg(sdw_.GetSegmentationID())
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
	if (sdw_.IsValidSegment()){
		OmSegmentCache* segCache = sdw_.GetSegmentCache();
		OmSegmentIterator iter(segCache);
		iter.iterOverSegmentID(sdw_.FindRootID());

		OmSegment * seg = iter.getNextSegment();
		while(NULL != seg) {
			OmSegment* parent = seg->getParent();
			OmSegID parentID = 0;
			if(parent){
				parentID = parent->value();
			}
			const QString str = QString("%1 : %2, %3, %4")
				.arg(seg->value())
				.arg(parentID)
				.arg(seg->getThreshold())
				.arg(seg->size());
			printf("%s\n", qPrintable(str));
			seg = iter.getNextSegment();
		}
	}
}

void OmSegmentContextMenu::addGroups()
{
        boost::shared_ptr<OmGroups> groups = sdw_.GetSegmentation().GetGroups();
        OmGroupIDsSet set = groups->GetGroups(sdw_.getSegment()->getRootSegID());
        OmGroupID firstID = 0;
	QString groupsStr = "Groups: ";
        foreach(OmGroupID id, set) {
                if(!firstID) {
                        firstID = id;
                }
                OmGroup & group = groups->GetGroup(id);
                groupsStr += group.GetName() + " + ";

                printf("here\n");
        }
	addAction(groupsStr);
}

void OmSegmentContextMenu::addDisableAction()
{
	const OmSegID segid = sdw_.FindRootID();
	OmSegmentCache* segCache = sdw_.GetSegmentCache();

	if(segCache->isSegmentEnabled(segid)) {
		addAction("Disable Segment", this, SLOT(disableSegment()));
	}
}

void OmSegmentContextMenu::disableSegment()
{
	const OmSegID segid = sdw_.FindRootID();
	OmSegmentCache* segCache = sdw_.GetSegmentCache();

	segCache->setSegmentEnabled(segid, false);
}
