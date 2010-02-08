
#include "omSegmentMergeAction.h"

#include "segment/omSegmentEditor.h"

#include "volume/omVolume.h"
#include "volume/omSegmentation.h"

#define DEBUG 0

#pragma mark -
#pragma mark OmSegmentMergeAction Class
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
	OmSegmentation & r_segmentation = OmVolume::GetSegmentation(mMergeSegmentationId);
	const OmIds & r_selected_segments = r_segmentation.GetSelectedSegmentIds();

	//for all selected segments
	OmIds::iterator itr;
	for (itr = r_selected_segments.begin(); itr != r_selected_segments.end(); itr++) {
		//mapped values
		const SegmentDataSet & r_data_set = r_segmentation.GetValuesMappedToSegmentId(*itr);

		//map set of values to segment id
		mPrevSegmentIdToValueMap[*itr] = r_data_set;

		//merge into set
		mMergedDataSet.insert(r_data_set.begin(), r_data_set.end());
	}

	//prompt user for destination segment
	mDestinationSegmentId = *(r_selected_segments.begin());

}

#pragma mark
#pragma mark Action Methods
/////////////////////////////////
///////          Action Methods

/*
 *	Unmap the ids of all selected segments from their data values and map destination segment
 *	to the union of all data values.
 */
void
 OmSegmentMergeAction::Action()
{

	//get merging segmentation
	OmSegmentation & r_segmentation = OmVolume::GetSegmentation(mMergeSegmentationId);

	//for all elements in the map
	map < OmId, SegmentDataSet >::iterator itr;
	for (itr = mPrevSegmentIdToValueMap.begin(); itr != mPrevSegmentIdToValueMap.end(); itr++) {

		const OmId & r_segment_id = itr->first;
		const SegmentDataSet & r_data_set = itr->second;

		r_segmentation.UnMapValuesToSegmentId(r_segment_id, r_data_set);
	}

	//map all data to destination id
	r_segmentation.MapValuesToSegmentId(mDestinationSegmentId, mMergedDataSet);
}

/*
 *	Restore initial mapping of all selected segment ids to data values.
 */
void OmSegmentMergeAction::UndoAction()
{

	//get merging segmentation
	OmSegmentation & r_segmentation = OmVolume::GetSegmentation(mMergeSegmentationId);

	//unmap all data to destination id
	r_segmentation.UnMapValuesToSegmentId(mDestinationSegmentId, mMergedDataSet);

	//for all elements in the map
	map < OmId, SegmentDataSet >::iterator itr;
	for (itr = mPrevSegmentIdToValueMap.begin(); itr != mPrevSegmentIdToValueMap.end(); itr++) {

		const OmId & r_segment_id = itr->first;
		const SegmentDataSet & r_data_set = itr->second;

		r_segmentation.MapValuesToSegmentId(r_segment_id, r_data_set);
	}
}

string OmSegmentMergeAction::Description()
{
	return "Merge Segments";
}
