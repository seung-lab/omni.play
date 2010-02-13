
#include "omSegmentDataMap.h"
#include "system/omDebug.h"

#define DEBUG 0

#pragma mark -
#pragma mark Example Class
/////////////////////////////////
///////
///////          Example Class
///////

#pragma mark
#pragma mark Mapping Methods
/////////////////////////////////
///////          Mapping Methods

bool OmSegmentDataMap::IsValueMappedToSegmentId(SEGMENT_DATA_TYPE value)
{
	return mDataValueToId.count(value);
}

SEGMENT_DATA_TYPE OmSegmentDataMap::GetUnMappedValue()
{

	//try all values starting at 1
	SEGMENT_DATA_TYPE value = 1;
	while (NULL_SEGMENT_DATA != value) {

		if (IsValueMappedToSegmentId(value)) {
			//mapped so inc and continue
			++value;
			continue;
		} else {
			//not mapped so return value
			return value;
		}

		//could not find free value
		return NULL_SEGMENT_DATA;
	}
}

OmId OmSegmentDataMap::GetSegmentIdMappedToValue(SEGMENT_DATA_TYPE value)
{
	return mDataValueToId[value];
}

const SegmentDataSet & OmSegmentDataMap::GetValuesMappedToSegmentId(OmId omId)
{
	return mIdToDataValues[omId];
}

/*
 *	Map a given data value to a given segment id.
 */
void OmSegmentDataMap::MapValueToSegmentId(OmId omId, SEGMENT_DATA_TYPE value)
{
	SegmentDataSet temp_set;
	temp_set.insert(value);

	MapValuesToSegmentId(omId, temp_set);
}

void OmSegmentDataMap::MapValuesToSegmentId(OmId omId, const SegmentDataSet & values)
{
	//unmap old mapped values
	UnMapValuesToSegmentId(omId, mIdToDataValues[omId]);

	//for each data value
	SegmentDataSet::iterator itr;
	for (itr = values.begin(); itr != values.end(); itr++) {

		//append mIdToDataValues[omId] set with elements in values set
		mIdToDataValues[omId].insert(*itr);

		//map each data value to omId
		mDataValueToId[*itr] = omId;
	}

}

void OmSegmentDataMap::UnMapValuesToSegmentId(OmId omId, const SegmentDataSet & values)
{
	SegmentDataSet::iterator itr;
	for (itr = values.begin(); itr != values.end(); itr++) {

		//remove each data value from the associated set
		mIdToDataValues[omId].erase(*itr);

		//unmap each data value to omId
		mDataValueToId.erase(*itr);
	}
}

/*
 *	Remove all mappings for given segment id.
 */
void OmSegmentDataMap::UnMapSegmentId(OmId omId)
{

	//all values mapped to id
	SegmentDataSet & values = mIdToDataValues[omId];

	//for each data value
	SegmentDataSet::iterator itr;
	for (itr = values.begin(); itr != values.end(); itr++) {

		//unmap values to id
		mDataValueToId.erase(*itr);
	}

	//unmap id to values
	mIdToDataValues.erase(omId);

}

void OmSegmentDataMap::UnMapValues(const SegmentDataSet & values)
{

	//for each data value
	SegmentDataSet::iterator itr;
	for (itr = values.begin(); itr != values.end(); itr++) {

		if (mDataValueToId.count(*itr) == 0)
			assert(false);

		//get mapping to id
		OmId segment_id = mDataValueToId[*itr];

		//remove id to data mapping
		(mIdToDataValues[segment_id]).erase(*itr);

		//unmap values to id
		mDataValueToId.erase(*itr);
	}

}
