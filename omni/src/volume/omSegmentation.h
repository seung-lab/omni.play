#ifndef OM_SEGMENTATION_H
#define OM_SEGMENTATION_H

/*
 *
 *	Brett Warne - bwarne@mit.edu - 3/9/09
 */


#include "omMipVolume.h"

#include "mesh/omMipMeshManager.h"
#include "voxel/omMipVoxelationManager.h"
#include "mesh/meshingManager.h"
#include "segment/omSegmentManager.h"
#include "system/omSystemTypes.h"
#include "system/omGenericManager.h"
#include "system/omManageableObject.h"
#include "system/events/omSystemModeEvent.h"
#include "volume/omSegmentationChunkCoord.h"
#include "common/omStd.h"

#include <vmmlib/frustumCuller.h>
using namespace vmml;



class OmVolumeCuller;
class OmSegment;

//template < class T > class OmGenericManager;



class OmSegmentation : public OmMipVolume, public OmManageableObject, public OmSystemModeEventListener {

public:
	OmSegmentation();
	OmSegmentation(OmId id);
	
	//properties
	void SetDirectoryPath(string dpath);
	
	
	//data mapping
	OmId GetSegmentIdMappedToValue( SEGMENT_DATA_TYPE value );
	const SegmentDataSet& GetValuesMappedToSegmentId( OmId );
	SEGMENT_DATA_TYPE GetValueMappedToSegmentId( OmId );
	void MapValuesToSegmentId( OmId, const SegmentDataSet & );
	void UnMapValuesToSegmentId( OmId, const SegmentDataSet & );
	
	
	//data accessor
	void SetVoxelValue(const DataCoord &, uint32_t);
	OmId GetVoxelSegmentId(const DataCoord &vox);
	void SetVoxelSegmentId(const DataCoord &vox, OmId omId);


	//build methods
	bool IsVolumeDataBuilt();
	void BuildVolumeData();
	
	bool IsMeshDataBuilt();
	void BuildMeshData();
	void BuildMeshDataPlan(const QString &);
	void BuildMeshChunk(int level, int x, int y, int z, int numThreads = 0);
	void BuildMeshDataInternal();	
	void QueueUpMeshChunk(OmSegmentationChunkCoord chunk_coord );
	void RunMeshQueue();

	void BuildChunk( const OmMipChunkCoord &mipCoord);
	void RebuildChunk(const OmMipChunkCoord &mipCoord, const SegmentDataSet &rEditedVals);
	
	
	
	//export
	void ExportDataFilter(vtkImageData *);
	
	//events
	void SystemModeChangeEvent(OmSystemModeEvent *event);
							
	//segment management
	OmSegment& GetSegment(OmId id);
	OmSegment& AddSegment();
	void RemoveSegment(OmId id);
	bool IsSegmentValid(OmId id);
	const OmIds& GetValidSegmentIds();
	bool IsSegmentEnabled(OmId id);
	void SetSegmentEnabled(OmId id, bool enable);
	void SetAllSegmentsEnabled(bool selected);
	const OmIds& GetEnabledSegmentIds();
	bool IsSegmentSelected(OmId id);
	void SetSegmentSelected(OmId id, bool selected);
	void SetAllSegmentsSelected(bool selected);
	const OmIds& GetSelectedSegmentIds();
	
	//segment data management
	const SegmentDataSet& GetEnabledSegmentDataValues();
	const SegmentDataSet& GetSelectedSegmentDataValues();
	const SegmentDataSet& GetUnselectedSegmentDataValues();
	const SegmentDataSet& GetVoxelizedSegmentDataValues();
	
	
	//drawing
	void Draw(const OmVolumeCuller &);
	void DrawChunkRecursive(const OmMipChunkCoord &, const SegmentDataSet &, bool testVis, const OmVolumeCuller &, const int numSegments);
	void DrawChunk(const OmMipChunkCoord &, const SegmentDataSet &, const OmVolumeCuller &rCuller);
	void DrawChunkMeshes( const OmMipChunkCoord &, const SegmentDataSet &, const OmBitfield & );
	void DrawChunkVoxels( const OmMipChunkCoord &, const SegmentDataSet &, const OmBitfield & );
	
	void DrawMeshes( const OmBitfield &, const OmMipChunkCoord &, const SegmentDataSet & ) { }
	
	void Print();
	OmMipMeshManager mMipMeshManager;

protected:
	//protected copy constructor and assignment operator to prevent copy
	OmSegmentation(const OmSegmentation&);
	OmSegmentation& operator= (const OmSegmentation&);
	
	
private:
	MeshingManager * mMeshingMan;

	//mesh data exists
	bool mIsMeshDataBuilt;
	
	//managers
	OmSegmentManager mSegmentManager;

	OmMipVoxelationManager mMipVoxelationManager;
	

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int file_version);
};






/////////////////////////////////
///////		 Serialization


BOOST_CLASS_VERSION(OmSegmentation, 0)

template<class Archive>
void 
OmSegmentation::serialize(Archive & ar, const unsigned int file_version) {
	ar & boost::serialization::base_object< OmMipVolume >(*this);
	ar & boost::serialization::base_object<OmManageableObject>(*this);
	
	ar & mSegmentManager;
	ar & mMipMeshManager;
}



#endif
