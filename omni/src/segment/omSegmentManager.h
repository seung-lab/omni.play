#ifndef OM_SEGMENT_MANAGER_H
#define OM_SEGMENT_MANAGER_H

/*
 *	Manages OmSegments and provides a cache for MipMeshes.  This is the interface
 *	between the OmSegmentation in the Volume library, OmSegments in the Segment library,
 *	and OmMipMesh in the Mesh library.
 *
 *	Brett Warne - bwarne@mit.edu - 3/9/09
 */

#include "omSegment.h"
#include "omSegmentDataMap.h"

#include "mesh/omMeshTypes.h"
#include "mesh/omMipMesh.h"
#include "mesh/omMipMeshManager.h"

#include "volume/omMipChunkCoord.h"

#include "system/omGenericManager.h"

#include "common/omStd.h"

class OmSegment;
class OmMipChunk;
class QGLContext;
class OmVolumeCuller;

class OmSegmentManager {

 public:
	OmSegmentManager();

	//properties
	const string & GetDirectoryPath();
	void SetDirectoryPath(const string &);

	//segment management
	 OmSegment & GetSegment(OmId id);
	 OmSegment & AddSegment();
	 OmSegment & AddSegment(SEGMENT_DATA_TYPE data_value);
	void AddSegments(const SegmentDataSet & data_values);
	void RemoveSegment(OmId id);
	void JoinSegments(OmId id1, OmId id2);

	bool IsSegmentValid(OmId omId) const;
	const OmIds & GetValidSegmentIds();

	bool IsSegmentEnabled(OmId id);
	void SetSegmentEnabled(OmId id, bool enable);
	void SetAllSegmentsEnabled(bool selected);
	const OmIds & GetEnabledSegmentIds();

	bool IsSegmentSelected(OmId id);
	void SetSegmentSelected(OmId id, bool selected);
	void SetAllSegmentsSelected(bool selected);
	const OmIds & GetSelectedSegmentIds();

	uint32_t SegmentSize();
	void SegmentCall(void (OmSegment::*fxn) ());

	//data map
	bool IsValueMappedToSegmentId(SEGMENT_DATA_TYPE value);
	OmId GetSegmentIdMappedToValue(SEGMENT_DATA_TYPE value);
	const SegmentDataSet & GetValuesMappedToSegmentId(OmId);

	void MapValueToSegmentId(OmId, SEGMENT_DATA_TYPE);
	void MapValuesToSegmentId(OmId, const SegmentDataSet &);
	void UnMapValuesToSegmentId(OmId, const SegmentDataSet &);

	//data values
	void ConvertSegmentIdsToDataValues(const OmIds &, SegmentDataSet &);
	void RefreshCachedSegmentDataValues();
	const SegmentDataSet & GetEnabledSegmentDataValues();
	const SegmentDataSet & GetSelectedSegmentDataValues();
	const SegmentDataSet & GetUnselectedSegmentDataValues();

 private:
	 string mDirectoryPath;

	//segment management
	 OmGenericManager < OmSegment > mGenericSegmentManager;

	//segment data mapping
	OmSegmentDataMap mSegmentDataMap;

	//cached value sets
	bool mCachedSegmentDataValuesDirty;
	SegmentDataSet mEnabledSegmentDataValues;
	SegmentDataSet mSelectedSegmentDataValues;
	SegmentDataSet mUnselectedSegmentDataValues;

	friend class boost::serialization::access;
	 template < class Archive > void serialize(Archive & ar, const unsigned int file_version);
};

#pragma mark
#pragma mark Serialization
/////////////////////////////////
///////          Serialization

BOOST_CLASS_VERSION(OmSegmentManager, 0)

template < class Archive > void
 OmSegmentManager::serialize(Archive & ar, const unsigned int file_version)
{
	ar & mDirectoryPath;

	ar & mGenericSegmentManager;
	ar & mSegmentDataMap;
}

#endif
