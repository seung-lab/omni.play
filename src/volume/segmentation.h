#pragma once

/*
 *
 * Brett Warne - bwarne@mit.edu - 3/9/09
 */

#include "common/om.hpp"
#include "datalayer/omDataWrapper.h"
#include "mesh/omMeshTypes.h"
#include "system/omManageableObject.h"
#include "volume/mipVolume.h"
#include "datalayer/archive/segmentation.h"

class OmChunk;
class OmChunkUniqueValuesManager;
class OmGroups;
class OmMST;
class OmMeshDrawer;
class OmMeshManager;
class OmMeshManagers;
class OmRawSegTileCache;
class OmSegChunk;
class OmSegment;
class OmSegmentLists;
class OmSegments;
class OmTileCacheSegmentation;
class OmUserEdges;
class OmValidGroupNum;
class OmViewGroupState;
class OmVolumeCuller;
class volumeData;
class SegmentationDataWrapper;
template <typename,typename> class OmChunkCache;

namespace om { namespace segmentation { class folder; } }
namespace om { namespace segmentation { class loader; } }
namespace om { namespace annotation { class manager; } }

class segmentation : public mipVolume, public OmManageableObject {
public:
    segmentation();
    segmentation(OmID id);
    virtual ~segmentation();

    std::string GetName();
    std::string GetNameHyphen();

    std::string GetDirectoryPath() const;
    void LoadPath();

    bool LoadVolData();
    bool LoadVolDataIfFoldersExist();
    void UpdateFromVolResize();

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
    
    void SetSizeThreshold( double t );
    double GetSizeThreshold();

    OmMeshManager* MeshManager(const double threshold);

    void UpdateVoxelBoundingData();

    void SetVolDataType(const OmVolDataType);

    void BuildBlankVolume(const Vector3i & dims);

    OmSegChunk* GetChunk(const om::chunkCoord& coord);

    uint32_t GetVoxelValue(const om::globalCoord &vox);
    void SetVoxelValue(const om::globalCoord &vox, const uint32_t value);
    bool SetVoxelValueIfSelected(const om::globalCoord &vox, const uint32_t value);

    void RebuildSegments();

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
    inline OmMeshManagers* MeshManagers(){
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
    inline volumeData* VolData(){
        return volData_.get();
    }
    inline OmRawSegTileCache* SliceCache(){
        return volSliceCache_.get();
    }
    inline OmChunkCache<segmentation, OmSegChunk>* ChunkCache(){
        return chunkCache_.get();
    }
    inline OmTileCacheSegmentation* TileCache(){
        return tileCache_.get();
    }
    inline om::segmentation::folder* Folder() const {
        return folder_.get();
    }
    inline om::annotation::manager* Annotations() const {
        return annotations_.get();
    }
    inline om::segmentation::loader* Loader() const {
        return loader_.get();
    }

private:
    boost::scoped_ptr<om::segmentation::folder> folder_;
    boost::scoped_ptr<om::segmentation::loader> loader_;
    boost::scoped_ptr<OmChunkUniqueValuesManager> uniqueChunkValues_;
    boost::scoped_ptr<OmGroups> groups_;
    boost::scoped_ptr<OmMST> mst_;
    boost::scoped_ptr<OmMeshDrawer> meshDrawer_;
    boost::scoped_ptr<OmMeshManagers> meshManagers_;
    boost::scoped_ptr<OmChunkCache<segmentation, OmSegChunk> > chunkCache_;
    boost::scoped_ptr<OmSegments> segments_;
    boost::scoped_ptr<OmSegmentLists> segmentLists_;
    boost::scoped_ptr<OmUserEdges> mstUserEdges_;
    boost::scoped_ptr<OmValidGroupNum> validGroupNum_;
    boost::scoped_ptr<volumeData> volData_;
    boost::scoped_ptr<OmRawSegTileCache> volSliceCache_;
    boost::scoped_ptr<OmTileCacheSegmentation> tileCache_;
    boost::scoped_ptr<om::annotation::manager> annotations_;

    template <class T> friend class OmVolumeBuilder;
    template <class T> friend class OmVolumeBuilderHdf5;
    template <class T> friend class OmVolumeBuilderImages;
    template <class T> friend class OmVolumeImporter;

    friend class OmSegmentsImpl;
    friend class OmSegmentsImplLowLevel;
    friend class OmSegmentIterator;
    friend class segmentationChunkBuildTask;
    friend class SegmentTests1;

    friend class OmDataArchiveProjectImpl;
    friend void YAML::operator>>(const YAML::Node& in, segmentation&);
    friend YAML::Emitter &YAML::operator<<(YAML::Emitter& out, const segmentation&);
    friend QDataStream &operator>>(QDataStream& in, segmentation&);
    friend QDataStream &operator<<(QDataStream& out, const segmentation&);
};

