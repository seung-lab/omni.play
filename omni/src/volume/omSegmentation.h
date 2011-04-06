#ifndef OM_SEGMENTATION_H
#define OM_SEGMENTATION_H

/*
 *
 * Brett Warne - bwarne@mit.edu - 3/9/09
 */

#include "chunks/omChunkTypes.hpp"
#include "common/om.hpp"
#include "datalayer/omDataWrapper.h"
#include "mesh/omMeshTypes.h"
#include "system/omManageableObject.h"
#include "volume/omMipVolume.h"

class OmChunkUniqueValuesManager;
class OmGroups;
class OmMST;
class OmMeshDrawer;
class OmMipMeshManager;
class OmMipMeshManagers;
class OmSegment;
class OmSegments;
class OmSegmentLists;
class OmUserEdges;
class OmValidGroupNum;
class OmViewGroupState;
class OmVolSliceCache;
class OmVolumeCuller;
class OmVolumeData;
class SegmentationDataWrapper;
template <typename,typename> class OmChunkCache;

class OmSegmentation : public OmMipVolume, public OmManageableObject {
public:
    OmSegmentation();
    OmSegmentation(OmID id);
    virtual ~OmSegmentation();

    std::string GetName();
    std::string GetNameHyphen();

    std::string GetDirectoryPath();

    void loadVolData();
    void loadVolDataIfFoldersExist();

    inline ObjectType getVolumeType() const {
        return SEGMENTATION;
    }

    inline OmID getID() const {
        return GetID();
    }

    virtual int GetBytesPerVoxel() const;
    virtual int GetBytesPerSlice() const;

    SegmentationDataWrapper GetSDW() const;

    void CloseDownThreads();

    void Flush();

    void SetDendThreshold( double t );
    double GetDendThreshold();

    OmMipMeshManager* MeshManager(const double threshold);

    void UpdateVoxelBoundingData();

    void SetVolDataType(const OmVolDataType);

    void BuildBlankVolume(const Vector3i & dims);

    void GetChunk(OmChunkPtr& ptr, const OmChunkCoord& coord);
    void GetChunk(OmSegChunkPtr& ptr, const OmChunkCoord& coord);

    uint32_t GetVoxelValue(const DataCoord &vox);
    void SetVoxelValue(const DataCoord &vox, const uint32_t value);

public:
    inline OmChunkUniqueValuesManager* ChunkUniqueValues(){
        return uniqueChunkValues_.get();
    }
    inline OmGroups* Groups(){
        return groups_.get();
    }
    inline OmMST* MST(){
        return mst_.get();
    }
    inline OmMeshDrawer* MeshDrawer(){
        return meshDrawer_.get();
    }
    inline OmMipMeshManagers* MeshManagers(){
        return meshManagers_.get();
    }
    inline OmSegments* Segments(){
        return segments_.get();
    }
    inline OmSegmentLists* SegmentLists(){
        return segmentLists_.get();
    }
    inline OmUserEdges* MSTUserEdges(){
        return mstUserEdges_.get();
    }
    inline OmValidGroupNum* ValidGroupNum(){
        return validGroupNum_.get();
    }
    inline OmVolumeData* VolData(){
        return volData_.get();
    }
    inline OmVolSliceCache* SliceCache(){
        return volSliceCache_.get();
    }
    inline OmChunkCache<OmSegmentation, OmSegChunk>* ChunkCache(){
        return chunkCache_.get();
    }

private:
    boost::scoped_ptr<OmChunkUniqueValuesManager> uniqueChunkValues_;
    boost::scoped_ptr<OmGroups> groups_;
    boost::scoped_ptr<OmMST> mst_;
    boost::scoped_ptr<OmMeshDrawer> meshDrawer_;
    boost::scoped_ptr<OmMipMeshManagers> meshManagers_;
    boost::scoped_ptr<OmChunkCache<OmSegmentation, OmSegChunk> > chunkCache_;
    boost::scoped_ptr<OmSegments> segments_;
    boost::scoped_ptr<OmSegmentLists> segmentLists_;
    boost::scoped_ptr<OmUserEdges> mstUserEdges_;
    boost::scoped_ptr<OmValidGroupNum> validGroupNum_;
    boost::scoped_ptr<OmVolumeData> volData_;
    boost::scoped_ptr<OmVolSliceCache> volSliceCache_;

    template <class T> friend class OmVolumeBuilder;
    template <class T> friend class OmVolumeBuilderHdf5;
    template <class T> friend class OmVolumeBuilderImages;
    template <class T> friend class OmVolumeImporter;

    friend class OmSegmentsImpl;
    friend class OmSegmentsImplLowLevel;
    friend class OmSegmentIterator;
    friend class OmSegmentationChunkBuildTask;
    friend class SegmentTests1;

    friend class OmDataArchiveProject;
    friend QDataStream &operator>>(QDataStream& in, OmSegmentation&);
    friend QDataStream &operator<<(QDataStream& out, const OmSegmentation&);
};

#endif
