#pragma once

#include "datalayer/dataWrapper.h"
#include "mesh/meshTypes.h"
#include "common/manageableObject.hpp"
#include "volume/mipVolume.h"
#include "datalayer/archive/segmentation.h"

namespace om {

namespace chunk {
class chunk;
class uniqueValuesManager;
class segChunk;
template <typename,typename> class chunkCache;
}

namespace mesh {
class manager;
class managers;
}

namespace segment {
class segment;
class lists;
class segments;
}

//class segmentationDataWrapper;

namespace annotation { class manager; }

namespace volume {

class data;
class dataType;
class segFolder;
class segLoader;

class segmentation : public volume, public common::manageableObject {
public:
    segmentation();
    segmentation(common::id id);
    virtual ~segmentation();

    std::string GetName();
    std::string GetNameHyphen();

    std::string GetDirectoryPath() const;
    void LoadPath();

    bool LoadVolData();
    bool LoadVolDataIfFoldersExist();
    void UpdateFromVolResize();

    inline common::objectType getVolumeType() const {
        return common::SEGMENTATION;
    }

    inline common::id getID() const {
        return GetID();
    }

    virtual int GetBytesPerVoxel() const;
    virtual int GetBytesPerSlice() const;

//    SegmentationDataWrapper GetSDW() const;

    void CloseDownThreads();

    void Flush();

    void SetDendThreshold( double t );
    double GetDendThreshold();

    void SetSizeThreshold( double t );
    double GetSizeThreshold();

    mesh::manager* MeshManager(const double threshold);

    void UpdateVoxelBoundingData();

    void SetVolDataType(const dataType);

    void BuildBlankVolume(const Vector3i & dims);

    chunk::segChunk* GetChunk(const coords::chunkCoord& coord);

    uint32_t GetVoxelValue(const coords::globalCoord &vox);
    void SetVoxelValue(const coords::globalCoord &vox, const uint32_t value);
    bool SetVoxelValueIfSelected(const coords::globalCoord &vox, const uint32_t value);

    void RebuildSegments();

public:
    inline chunk::uniqueValuesManager* ChunkUniqueValues() {
        return uniqueChunkValues_.get();
    }
    inline mesh::managers* MeshManagers() {
        return managers_.get();
    }
    inline segment::segments* Segments() {
        return segments_.get();
    }
    inline segment::lists* SegmentLists() {
        return segmentLists_.get();
    }
    inline data* VolData() {
        return volData_.get();
    }
    inline segFolder* Folder() const {
        return folder_.get();
    }
    inline annotation::manager* Annotations() const {
        return annotations_.get();
    }
    inline segLoader* Loader() const {
        return loader_.get();
    }

private:
    boost::scoped_ptr<segFolder> folder_;
    boost::scoped_ptr<segLoader> loader_;
    boost::scoped_ptr<chunk::uniqueValuesManager> uniqueChunkValues_;
    boost::scoped_ptr<mesh::managers> managers_;
    boost::scoped_ptr<segment::segments> segments_;
    boost::scoped_ptr<segment::lists> segmentLists_;
    boost::scoped_ptr<data> volData_;
    boost::scoped_ptr<annotation::manager> annotations_;

    friend class segmentsImpl;
    friend class segmentsImplLowLevel;
    friend class segmentIterator;
    friend class segmentationChunkBuildTask;

    friend void YAML::operator>>(const YAML::Node& in, segmentation&);
    friend YAML::Emitter &YAML::operator<<(YAML::Emitter& out, const segmentation&);
};

} // namespace volume
} // namespace om
