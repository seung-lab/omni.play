#pragma once

#include "datalayer/dataWrapper.h"
#include "mesh/meshTypes.h"
#include "common/manageableObject.hpp"
#include "volume/volume.h"
#include "datalayer/archive/segmentation.h"
#include "volume/segmentationFolder.h"
//#include "segment/segments.h"

namespace om {

namespace chunks {
class chunk;
class segChunk;
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

    inline common::objectType getVolumeType() const {
        return common::SEGMENTATION;
    }

    inline common::id getID() const {
        return GetID();
    }

    void SetVolDataType(const dataType type);

    virtual int GetBytesPerVoxel() const;
    virtual int GetBytesPerSlice() const;

    void Flush();

    boost::shared_ptr<chunks::segChunk> GetChunk(const coords::chunkCoord& coord);

    uint32_t GetVoxelValue(const coords::globalCoord &vox);
    void SetVoxelValue(const coords::globalCoord &vox, const uint32_t value);

    void RebuildSegments();

public:

    inline data* VolData() {
        return volData_.get();
    }

//    inline segment::segments * Segments() {
//        return segments_.get();
//    }

    inline om::segmentation::folder* Folder() const {
        return folder_.get();
    }

private:
    inline void setId(int id) {
        id_ = id;
    }

    boost::scoped_ptr<om::segmentation::folder> folder_;
    boost::scoped_ptr<data> volData_;
//    boost::scoped_ptr<segment::segments> segments_;

    friend class segmentsImpl;
    friend class segmentsImplLowLevel;
    friend class segmentIterator;
    friend class segmentationChunkBuildTask;
    template<typename T> friend class YAML::volume;

    friend void YAML::operator>>(const YAML::Node& in, segmentation&);
    friend YAML::Emitter &YAML::operator<<(YAML::Emitter& out, const segmentation&);
};

} // namespace volume
} // namespace om
