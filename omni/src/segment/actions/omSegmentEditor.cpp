#include "common/omDebug.h"
#include "omSegmentEditor.h"
#include "project/omProject.h"
#include "segment/omSegmentCache.h"
#include "system/events/omSegmentEvent.h"
#include "system/omEventManager.h"
#include "volume/omSegmentation.h"
#include "volume/omVolume.h"
#include "utility/dataWrappers.h"

//init instance pointer
OmSegmentEditor *OmSegmentEditor::mspInstance = 0;

OmSegmentEditor::OmSegmentEditor()
	: mEditSegmentation(0)
	, mEditSegment(0)
{
}

OmSegmentEditor::~OmSegmentEditor()
{
}

OmSegmentEditor *OmSegmentEditor::Instance()
{
	if (NULL == mspInstance) {
		mspInstance = new OmSegmentEditor();
	}
	return mspInstance;
}

void OmSegmentEditor::Delete()
{
	if (mspInstance)
		delete mspInstance;
	mspInstance = NULL;
}

/////////////////////////////////
///////          Edit Selection

void OmSegmentEditor::SetEditSelection(OmId segmentation, OmId segment)
{
	Instance()->mEditSegmentation = segmentation;
	Instance()->mEditSegment = segment;

	OmEventManager::PostEvent(new OmSegmentEvent(OmSegmentEvent::SEGMENT_EDIT_SELECTION_CHANGE));
}

SegmentDataWrapper OmSegmentEditor::GetEditSelection()
{
	return SegmentDataWrapper( Instance()->mEditSegmentation,
				   Instance()->mEditSegment );
}

/////////////////////////////////
///////          Selected Voxels

const set < DataCoord > & OmSegmentEditor::GetSelectedVoxels()
{
	return Instance()->mSelectedVoxels;
}

void OmSegmentEditor::SetSelectedVoxels(const set < DataCoord > &voxels)
{
	Instance()->mSelectedVoxels = voxels;
}

void OmSegmentEditor::ClearSelectedVoxels()
{
	Instance()->mSelectedVoxels.clear();
}

void OmSegmentEditor::SetSelectedVoxelState(const DataCoord & dataCoord, bool selected)
{
	if (selected)
		Instance()->mSelectedVoxels.insert(dataCoord);
	else
		Instance()->mSelectedVoxels.erase(dataCoord);
}

bool OmSegmentEditor::GetSelectedVoxelState(const DataCoord & dataCoord)
{
	return Instance()->mSelectedVoxels.count(dataCoord);
}

