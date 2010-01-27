
#include "omSegmentManager.h"
#include "omSegment.h"

#include "common/omGl.h"
#include "volume/omMipChunk.h"
#include "volume/omVolumeCuller.h"
#include "utility/setUtilities.h"

#include <vtkImageData.h>
#include "system/omDebug.h"

#define DEBUG 0

#pragma mark -
#pragma mark OmSegmentManager
/////////////////////////////////
///////
///////          OmSegmentManager
///////
OmSegmentManager::OmSegmentManager()
{

	//initially dirty cache
	mCachedSegmentDataValuesDirty = true;
}

#pragma mark
#pragma mark Segment Management
/////////////////////////////////
///////         Segment Management

OmSegment & OmSegmentManager::GetSegment(OmId id)
{
	return mGenericSegmentManager.Get(id);
}

OmSegment & OmSegmentManager::AddSegment()
{
	mCachedSegmentDataValuesDirty = true;

	//get an unmapped value
	SEGMENT_DATA_TYPE data_value = mSegmentDataMap.GetUnMappedValue();

	//check value already mapped or there are no more free values
	if (NULL_SEGMENT_DATA == data_value)
		assert(false);

	//try to add
	OmSegment & r_segment = mGenericSegmentManager.Add();

	//and map value
	MapValueToSegmentId(r_segment.GetId(), data_value);

	return r_segment;
}

OmSegment & OmSegmentManager::AddSegment(SEGMENT_DATA_TYPE data_value)
{
	mCachedSegmentDataValuesDirty = true;

	//check value already mapped or there are no more free values
	if (IsValueMappedToSegmentId(data_value))
		assert(false);

	//try to add
	OmSegment & r_segment = mGenericSegmentManager.Add();

	//and map value
	MapValueToSegmentId(r_segment.GetId(), data_value);

	return r_segment;
}

void OmSegmentManager::AddSegments(const SegmentDataSet & data_values)
{

	//for all root indirect data values
	SEGMENT_DATA_TYPE segment_data_value;
	SegmentDataSet::iterator itr;
	for (itr = data_values.begin(); itr != data_values.end(); itr++) {

		//store segment data value
		segment_data_value = *itr;

		//if data value is already mapped, then skip
		if (IsValueMappedToSegmentId(segment_data_value))
			continue;

		//else add a new segment
		AddSegment(segment_data_value);
	}
}

void OmSegmentManager::JoinSegments(OmId id1, OmId id2)
{

}

void OmSegmentManager::RemoveSegment(OmId id)
{
	mGenericSegmentManager.Remove(id);
}

bool OmSegmentManager::IsSegmentValid(OmId omId) const const
{
	return mGenericSegmentManager.IsValid(omId);
}

const set < OmId > & OmSegmentManager::GetValidSegmentIds()
{
	return mGenericSegmentManager.GetValidIds();
}

bool OmSegmentManager::IsSegmentEnabled(OmId id)
{
	return mGenericSegmentManager.IsEnabled(id);
}

void OmSegmentManager::SetSegmentEnabled(OmId id, bool enable)
{
	mCachedSegmentDataValuesDirty = true;
	mGenericSegmentManager.SetEnabled(id, enable);
}

const set < OmId > & OmSegmentManager::GetEnabledSegmentIds()
{
	return mGenericSegmentManager.GetEnabledIds();
}

bool OmSegmentManager::IsSegmentSelected(OmId id)
{
	return mGenericSegmentManager.IsSelected(id);
}

void OmSegmentManager::SetSegmentSelected(OmId id, bool selected)
{
	mCachedSegmentDataValuesDirty = true;
	mGenericSegmentManager.SetSelected(id, selected);
}

void OmSegmentManager::SetAllSegmentsSelected(bool selected)
{
	mCachedSegmentDataValuesDirty = true;
	mGenericSegmentManager.SetAllSelected(selected);
}

void OmSegmentManager::SetAllSegmentsEnabled(bool selected)
{
	mCachedSegmentDataValuesDirty = true;
	mGenericSegmentManager.SetAllEnabled(selected);
}

const OmIds & OmSegmentManager::GetSelectedSegmentIds()
{
	return mGenericSegmentManager.GetSelectedIds();
}

uint32_t OmSegmentManager::SegmentSize()
{
	return mGenericSegmentManager.Size();
}

void OmSegmentManager::SegmentCall(void (OmSegment::*fxn) ())
{
	mGenericSegmentManager.CallValid(fxn);
}

#pragma mark
#pragma mark DataMap Methods
/////////////////////////////////
///////          DataMap Methods

bool OmSegmentManager::IsValueMappedToSegmentId(SEGMENT_DATA_TYPE value)
{
	return mSegmentDataMap.IsValueMappedToSegmentId(value);
}

OmId OmSegmentManager::GetSegmentIdMappedToValue(SEGMENT_DATA_TYPE value)
{
	mSegmentDataMap.GetSegmentIdMappedToValue(value);
}

const SegmentDataSet & OmSegmentManager::GetValuesMappedToSegmentId(OmId omId)
{
	mSegmentDataMap.GetValuesMappedToSegmentId(omId);
}

void OmSegmentManager::MapValueToSegmentId(OmId omId, SEGMENT_DATA_TYPE value)
{
	mSegmentDataMap.MapValueToSegmentId(omId, value);
}

void OmSegmentManager::MapValuesToSegmentId(OmId omId, const SegmentDataSet & rValueSet)
{
	mSegmentDataMap.MapValuesToSegmentId(omId, rValueSet);
}

void OmSegmentManager::UnMapValuesToSegmentId(OmId omId, const SegmentDataSet & rValueSet)
{
	mSegmentDataMap.UnMapValuesToSegmentId(omId, rValueSet);
}

#pragma mark
#pragma mark Cached Segment Data Values Methods
/////////////////////////////////
///////          Cached Segment Data Values Methods

void OmSegmentManager::ConvertSegmentIdsToDataValues(const OmIds & omIds, SegmentDataSet & values)
{

	//clear data set
	values.clear();

	//for each omid in given set
	OmIds::iterator itr;
	for (itr = omIds.begin(); itr != omIds.end(); itr++) {
		//get set of values mapped to id
		const SegmentDataSet & mapped_data_vals = GetValuesMappedToSegmentId(*itr);
		//insert all mapped values
		values.insert(mapped_data_vals.begin(), mapped_data_vals.end());
	}

}

void OmSegmentManager::RefreshCachedSegmentDataValues()
{
	//convert enabled
	ConvertSegmentIdsToDataValues(GetEnabledSegmentIds(), mEnabledSegmentDataValues);

	//convert selected
	ConvertSegmentIdsToDataValues(GetSelectedSegmentIds(), mSelectedSegmentDataValues);

	//form unselected
	setDifference < SEGMENT_DATA_TYPE > (mEnabledSegmentDataValues, mSelectedSegmentDataValues,
					     mUnselectedSegmentDataValues);

	//mark cache clean
	mCachedSegmentDataValuesDirty = false;
}

const SegmentDataSet & OmSegmentManager::GetEnabledSegmentDataValues()
{
	if (mCachedSegmentDataValuesDirty)
		RefreshCachedSegmentDataValues();
	return mEnabledSegmentDataValues;
}

const SegmentDataSet & OmSegmentManager::GetSelectedSegmentDataValues()
{
	if (mCachedSegmentDataValuesDirty)
		RefreshCachedSegmentDataValues();
	return mSelectedSegmentDataValues;
}

const SegmentDataSet & OmSegmentManager::GetUnselectedSegmentDataValues()
{
	if (mCachedSegmentDataValuesDirty)
		RefreshCachedSegmentDataValues();
	return mUnselectedSegmentDataValues;
}

#pragma mark
#pragma mark Accessor Methods
/////////////////////////////////
///////          Accessor Methods

const string & OmSegmentManager::GetDirectoryPath()
{
	return mDirectoryPath;
}

void OmSegmentManager::SetDirectoryPath(const string & dpath)
{
	mDirectoryPath = dpath;
}
