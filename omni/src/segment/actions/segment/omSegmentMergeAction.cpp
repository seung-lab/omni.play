#include "omSegmentMergeAction.h"

#include "segment/omSegmentEditor.h"
#include "system/events/omSegmentEvent.h"

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

		OmSegment& segment = r_segmentation.GetSegment(*itr);
		if( 0 == segment.get_original_mapped_data_value() ) {
			if( 1 == r_data_set.size() ){
				OmIds::iterator value_iter;
				value_iter = r_data_set.begin();
				segment.set_original_mapped_data_value( *value_iter);
			}
		}
		    

		//map set of values to segment id
		mPrevSegmentIdToValueMap[*itr] = r_data_set;

		//merge into set
		mMergedDataSet.insert(r_data_set.begin(), r_data_set.end());
	}

	//TODO: prompt user for destination segment?
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

	//for all elements in the map
	char buffer[255];
	snprintf (buffer, 254, "Set to %i", mDestinationSegmentId);
	char merged_into_buffer[255];
	snprintf (merged_into_buffer, 254, "Merged into me");
	for (itr = mPrevSegmentIdToValueMap.begin(); itr != mPrevSegmentIdToValueMap.end(); itr++) {
		const OmId & r_segment_id = itr->first;
		const SegmentDataSet & r_data_set = itr->second;
		OmSegment & seg = r_segmentation.GetSegment (itr->first);
		if( seg.GetId() == mDestinationSegmentId ){
			seg.SetNote (merged_into_buffer);
		} else {
			seg.SetNote (buffer);
		}
		debug ("segment", "seg's id = %i\n", seg.GetId());
	}
	OmEventManager::PostEvent(new OmSegmentEvent(OmSegmentEvent::SEGMENT_OBJECT_MODIFICATION));
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
