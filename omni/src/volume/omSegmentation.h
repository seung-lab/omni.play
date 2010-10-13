#ifndef OM_SEGMENTATION_H
#define OM_SEGMENTATION_H

/*
 *
 *	Brett Warne - bwarne@mit.edu - 3/9/09
 */

#include "system/omManageableObject.h"
#include "volume/omMipVolume.h"
#include "datalayer/omDataWrapper.h"
#include "mesh/omMeshTypes.h"

class OmMipMeshManager;
class OmGroups;
class OmMST;
class OmSegmentLists;
class OmSegment;
class OmSegmentCache;
class OmViewGroupState;
class OmVolumeCuller;
class OmVolumeData;

class OmSegmentation : public OmMipVolume, public OmManageableObject {
public:
	OmSegmentation();
	OmSegmentation(OmId id);
	~OmSegmentation();

	boost::shared_ptr<OmVolumeData> getVolData();

	std::string GetName();
	std::string GetDirectoryPath();
	void loadVolData();
	ObjectType getVolumeType(){ return SEGMENTATION; }
	OmId getID(){ return GetID(); }
	OmMipVolumeCache* getDataCache(){ return mDataCache; }
	int GetBytesPerSample() const;

	void CloseDownThreads();

	void BuildVolumeData();
	void Mesh();
	void MeshChunk(const OmMipChunkCoord& coord);

	void RebuildChunk(const OmMipChunkCoord &, const OmSegIDsSet &);

	//segment management
	boost::shared_ptr<OmSegmentCache> GetSegmentCache(){
		return mSegmentCache;
	}
	boost::shared_ptr<OmSegmentLists> GetSegmentLists(){
		return mSegmentLists;
	}

	void GetMesh(OmMipMeshPtr& ptr, const OmMipChunkCoord&, const OmSegID );

	//group management
	boost::shared_ptr<OmGroups> GetGroups(){ return mGroups; }
 	void SetGroup(const OmSegIDsSet&, OmSegIDRootType, OmGroupName);
	void UnsetGroup(const OmSegIDsSet&, OmSegIDRootType, OmGroupName);

	void FlushDirtySegments();
	void FlushDend();
	void FlushDendUserEdges();
	void SetDendThreshold( float t );
	void SetDendThresholdAndReload( const float t );
	float GetDendThreshold();
	boost::shared_ptr<OmMST> getMST(){
		return mst_;
	}

	Vector3i FindCenterOfSelectedSegments() const;

	bool ImportSourceData(const OmDataPath& path);

	void SetVolDataType(const OmVolDataType);

protected:
	virtual void doBuildThreadedVolume();

private:
	OmMipVolumeCache *const mDataCache;

	void KillCacheThreads();

	boost::shared_ptr<OmVolumeData> mVolData;
	boost::shared_ptr<OmSegmentCache> mSegmentCache;
	boost::shared_ptr<OmSegmentLists> mSegmentLists;
	boost::shared_ptr<OmGroups> mGroups;
	boost::shared_ptr<OmMST> mst_;
	boost::shared_ptr<OmMipMeshManager> mMipMeshManager;

	OmDataWrapperPtr doExportChunk(const OmMipChunkCoord &);

	friend class OmBuildSegmentation;
	template <class T> friend class OmVolumeImporter;

	friend class OmSegmentCacheImpl;
	friend class OmSegmentCacheImplLowLevel;
	friend class OmSegmentIterator;
	friend class MstViewerImpl;
	friend class OmSegmentationChunkBuildTask;

	friend QDataStream &operator<<(QDataStream& out, const OmSegmentation&);
	friend QDataStream &operator>>(QDataStream& in, OmSegmentation &);
};

#endif
