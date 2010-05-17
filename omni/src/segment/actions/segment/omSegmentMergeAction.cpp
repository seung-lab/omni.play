#include "omSegmentMergeAction.h"
#include "segment/omSegmentEditor.h"
#include "system/events/omSegmentEvent.h"
#include "volume/omVolume.h"
#include "project/omProject.h"
#include "volume/omSegmentation.h"

/////////////////////////////////
///////
///////          OmSegmentMergeAction Class
///////

/*
 *	MergeAction constructor stores the id of segmentation containing selected segments being
 *	merged (merge segmentation), the destination segment id, and a map of all selected segments
 *	to their segment data values.
 */
OmSegmentMergeAction::OmSegmentMergeAction(OmId mergeSegmentationId)
{
	//segmentation where selected segements will be merged
	mMergeSegmentationId = mergeSegmentationId;

	//copy all selected segments and their set of data vals
	OmSegmentation & segmentation = OmProject::GetSegmentation(mMergeSegmentationId);
	QList<OmId> selected_segments = segmentation.GetSelectedSegmentIds().toList();

	if( 0 == selected_segments.size() ){
		return;
	}

	OmSegment* base = segmentation.GetSegment(selected_segments.takeFirst());
	foreach( OmId segID, selected_segments ){
		base->Join( segmentation.GetSegment(segID) );
	}

	//TODO: prompt user for destination segment?
	mDestinationSegmentId = base->getValue();

}

/////////////////////////////////
///////          Action Methods

/*
 *	Unmap the ids of all selected segments from their data values and map destination segment
 *	to the union of all data values.
 */
void OmSegmentMergeAction::Action()
{
}

/*
 *	Restore initial mapping of all selected segment ids to data values.
 */
void OmSegmentMergeAction::UndoAction()
{
}

string OmSegmentMergeAction::Description()
{
	return "Merge Segments";
}
