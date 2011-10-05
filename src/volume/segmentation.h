#pragma once

/*
 *
 * Brett Warne - bwarne@mit.edu - 3/9/09
 */

#include "common/om.hpp"
#include "datalayer/dataWrapper.h"
#include "mesh/meshTypes.h"
#include "system/omManageableObject.h"
#include "volume/mipVolume.h"
#include "datalayer/archive/segmentation.h"

class chunk;
class chunkUniqueValuesManager;
class OmGroups;
class OmMST;
class meshDrawer;
class meshManager;
class meshManagers;
class OmRawSegTileCache;
class segChunk;
class segment;
class segmentLists;
class segments;
class tileCacheSegmentation;
class OmUserEdges;
class OmValidGroupNum;
class OmViewGroupState;
class OmVolumeCuller;
class volumeData;
class SegmentationDataWrapper;
template <typename,typename> class chunkCache;

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

    meshManager* MeshManager(const double threshold);

    void UpdateVoxelBoundingData();

    void SetVolDataType(const OmVolDataType);

    void BuildBlankVolume(const Vector3i & dims);

    segChunk* GetChunk(const om::chunkCoord& coord);

    uint32_t GetVoxelValue(const om::globalCoord &vox);
    void SetVoxelValue(const om::globalCoord &vox, const uint32_t value);
    bool SetVoxelValueIfSelected(const om::globalCoord &vox, const uint32_t value);

    void RebuildSegments();

public:
    inline chunkUniqueValuesManager* ChunkUniqueValues(){
        return uniqueChunkValues_.get();
    }
    inline OmGroups* Groups(){
        return groups_.get();
    }
    inline OmMST* MST(){
        return mst_.get();
    }
    inline meshDrawer* MeshDrawer(){
        return meshDrawer_.get();
    }
    inline meshManagers* MeshManagers(){
        return meshManagers_.get();
    }
    inline segments* Segments(){
        return segments_.get();
    }
    inline segmentLists* SegmentLists(){
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
    inline chunkCache<segmentation, segChunk>* ChunkCache(){
        return chunkCache_.get();
    }
    inline tileCacheSegmentation* TileCache(){
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
    boost::scoped_ptr<chunkUniqueValuesManager> uniqueChunkValues_;
    boost::scoped_ptr<OmGroups> groups_;
    boost::scoped_ptr<OmMST> mst_;
    boost::scoped_ptr<meshDrawer> meshDrawer_;
    boost::scoped_ptr<meshManagers> meshManagers_;
    boost::scoped_ptr<chunkCache<segmentation, segChunk> > chunkCache_;
    boost::scoped_ptr<segments> segments_;
    boost::scoped_ptr<segmentLists> segmentLists_;
    boost::scoped_ptr<OmUserEdges> mstUserEdges_;
    boost::scoped_ptr<OmValidGroupNum> validGroupNum_;
    boost::scoped_ptr<volumeData> volData_;
    boost::scoped_ptr<OmRawSegTileCache> volSliceCache_;
    boost::scoped_ptr<tileCacheSegmentation> tileCache_;
    boost::scoped_ptr<om::annotation::manager> annotations_;

    template <class T> friend class OmVolumeBuilder;
    template <class T> friend class OmVolumeBuilderHdf5;
    template <class T> friend class OmVolumeBuilderImages;
    template <class T> friend class OmVolumeImporter;

    friend class segmentsImpl;
    friend class segmentsImplLowLevel;
    friend class segmentIterator;
    friend class segmentationChunkBuildTask;
    friend class SegmentTests1;

    friend class dataArchiveProjectImpl;
    friend void YAML::operator>>(const YAML::Node& in, segmentation&);
    friend YAML::Emitter &YAML::operator<<(YAML::Emitter& out, const segmentation&);
    friend QDataStream &operator>>(QDataStream& in, segmentation&);
    friend QDataStream &operator<<(QDataStream& out, const segmentation&);
};

