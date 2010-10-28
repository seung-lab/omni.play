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
class OmUserEdges;
class OmSegmentLists;
class OmSegment;
class OmSegmentCache;
class OmViewGroupState;
class OmVolumeCuller;
class OmVolumeData;
class SegmentationDataWrapper;

class OmSegmentation : public OmMipVolume, public OmManageableObject {
public:
	OmSegmentation();
	OmSegmentation(OmID id);
	~OmSegmentation();

	boost::shared_ptr<OmVolumeData> getVolData();

	std::string GetName();
	std::string GetDirectoryPath();
	void loadVolData();
	ObjectType getVolumeType(){ return SEGMENTATION; }
	OmID getID(){ return GetID(); }
	OmMipVolumeCache* getDataCache(){ return mDataCache; }
	int GetBytesPerSample() const;

	SegmentationDataWrapper getSDW() const;

	void CloseDownThreads();

	void Flush();
	void BuildVolumeData();
	void Mesh();
	void MeshChunk(const OmMipChunkCoord& coord);

	void RebuildChunk(const OmMipChunkCoord &, const OmSegIDsSet &);

	//segment management
	OmSegmentCache* GetSegmentCache(){
		return mSegmentCache;
	}
	boost::shared_ptr<OmSegmentLists> GetSegmentLists(){
		return mSegmentLists;
	}

	void GetMesh(OmMipMeshPtr& ptr, const OmMipChunkCoord&, const OmSegID );

	//group management
	boost::shared_ptr<OmGroups> GetGroups(){ return mGroups; }

	void SetDendThreshold( double t );
	double GetDendThreshold();
	boost::shared_ptr<OmMST> getMST(){
		return mst_;
	}
	boost::shared_ptr<OmUserEdges> getMSTUserEdges(){
		return mstUserEdges_;
	}

	DataCoord FindCenterOfSelectedSegments() const;

	bool ImportSourceData(const OmDataPath& path);

	void SetVolDataType(const OmVolDataType);

	boost::shared_ptr<std::set<OmSegment*> >
	GetAllChildrenSegments(const OmSegIDsSet& set);

	void BuildBlankVolume(const Vector3i & dims);

protected:
	virtual void doBuildThreadedVolume();

private:
	OmMipVolumeCache *const mDataCache;

	void KillCacheThreads();

	boost::shared_ptr<OmVolumeData> mVolData;
	OmSegmentCache* mSegmentCache;
	boost::shared_ptr<OmSegmentLists> mSegmentLists;
	boost::shared_ptr<OmGroups> mGroups;
	boost::shared_ptr<OmMST> mst_;
	boost::shared_ptr<OmUserEdges> mstUserEdges_;
	boost::shared_ptr<OmMipMeshManager> mMipMeshManager;

	OmDataWrapperPtr doExportChunk(const OmMipChunkCoord &,
								   const bool rerootSegments);

	friend class OmBuildSegmentation;
	template <class T> friend class OmVolumeImporter;

	friend class OmSegmentCacheImpl;
	friend class OmSegmentCacheImplLowLevel;
	friend class OmSegmentIterator;
	friend class MstViewerImpl;
	friend class OmSegmentationChunkBuildTask;
	friend class SegmentTests;

	friend QDataStream &operator<<(QDataStream& out, const OmSegmentation&);
	friend QDataStream &operator>>(QDataStream& in, OmSegmentation &);
};

#endif
