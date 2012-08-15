#pragma once

#include "volume/omVolCoords.hpp"
#include "datalayer/archive/channel.h"

class OmMipVolume;

class OmMipVolCoords : public OmVolCoords {
private:
    int mMipLeafDim; //must be even

    //inferred from leaf dim and source data extent
    int mMipRootLevel;

    const OmMipVolume * const vol_;

    template <class T> friend class OmMipVolumeArchive;

    friend class OmDataArchiveProject;
    friend class OmMipVolumeArchiveOld;

    friend YAML::Emitter& YAML::operator<<(YAML::Emitter& out, const OmMipVolCoords& c);
    friend void YAML::operator>>(const YAML::Node& in, OmMipVolCoords& c);
    friend QDataStream& operator<<(QDataStream& out, const OmMipVolCoords& c);
    friend QDataStream& operator>>(QDataStream& in, OmMipVolCoords& c);


public:
    OmMipVolCoords(const OmMipVolume * const vol)
        : mMipLeafDim(0)
        , mMipRootLevel(0)
        , vol_(vol)
    {}

    virtual ~OmMipVolCoords()
    {}

//mip level method

    // Calculate MipRootLevel using GetChunkDimension().
    void UpdateRootLevel()
    {
        //determine max level
        Vector3i source_dims = GetDataExtent().getUnitDimensions();
        int max_source_dim = source_dims.getMaxComponent();
        int mipchunk_dim = GetChunkDimension();

        if (max_source_dim <= mipchunk_dim) {
            mMipRootLevel = 0;
        } else {
            mMipRootLevel = ceil(log((float) (max_source_dim) / mipchunk_dim) / log(2.0f));
        }
    }

    inline int GetRootMipLevel() const {
        return mMipRootLevel;
    }

    inline Vector3i MipedDataDimensions(const int level) const
    {
        return GetDataDimensions() / om::math::pow2int(level);
    }

    // Calculate the data dimensions needed to contain the volume at a given compression level.
    // TODO: should this be factored out?
    inline Vector3i MipLevelDataDimensions(const int level) const
    {
        //get dimensions
        om::globalBbox source_extent = GetDataExtent();
        Vector3f source_dims = source_extent.getUnitDimensions();

        //dims in fraction of pixels
        Vector3f mip_level_dims = source_dims / om::math::pow2int(level);

        return Vector3i(ceil(mip_level_dims.x),
                        ceil(mip_level_dims.y),
                        ceil(mip_level_dims.z));
    }

    // Calculate the MipChunkCoord dims required to contain all the chunks of a given level.
    // TODO: this appear to NOT do the right thing for level=0 (purcaro)
    inline Vector3i MipLevelDimensionsInMipChunks(int level) const
    {
        const Vector3f data_dims = MipLevelDataDimensions(level);
        return Vector3i (ceil(data_dims.x / GetChunkDimension()),
                         ceil(data_dims.y / GetChunkDimension()),
                         ceil(data_dims.z / GetChunkDimension()));
    }

    inline Vector3i GetChunkDimensions() const
    {
        return Vector3i(GetChunkDimension(),
                        GetChunkDimension(),
                        GetChunkDimension());
    }

    //mip chunk methods
    inline om::chunkCoord RootMipChunkCoordinate() const {
        return om::chunkCoord(mMipRootLevel, Vector3i::ZERO);
    }

    // Returns true if given MipCoordinate is a valid coordinate within the MipVolume.
    inline bool ContainsMipChunkCoord(const om::chunkCoord & rMipCoord) const
    {
        //if level is greater than root level
        if(rMipCoord.Level < 0 ||
           rMipCoord.Level > GetRootMipLevel()){
            return false;
        }

        //convert to data box in leaf (MIP 0)
        om::globalBbox bbox = rMipCoord.chunkBoundingBox(vol_).toGlobalBbox();

        bbox.intersect(GetDataExtent());
        if(bbox.isEmpty()){
            return false;
        }

        //else valid mip coord
        return true;
    }

    // Finds set of children coordinates that are valid for this MipVolume.
    inline void ValidMipChunkCoordChildren(const om::chunkCoord & rMipCoord,
                                           std::set<om::chunkCoord>& children) const
    {
        //clear set
        children.clear();

        //get all possible children
        om::chunkCoord possible_children[8];
        rMipCoord.ChildrenCoords(possible_children);

        //for all possible children
        for (int i = 0; i < 8; i++) {
            if (ContainsMipChunkCoord(possible_children[i])) {
                children.insert(possible_children[i]);
            }
        }
    }

    inline Vector3i getDimsRoundedToNearestChunk(const int level) const
    {
        const Vector3i data_dims = MipLevelDataDimensions(level);

        return Vector3i(om::math::roundUp(data_dims.x, GetChunkDimension()),
                        om::math::roundUp(data_dims.y, GetChunkDimension()),
                        om::math::roundUp(data_dims.z, GetChunkDimension()));
    }

    inline uint32_t ComputeTotalNumChunks() const
    {
        uint32_t numChunks = 0;

        for (int level = 0; level <= GetRootMipLevel(); ++level) {
            numChunks += ComputeTotalNumChunks(level);
        }

        return numChunks;
    }

    inline uint32_t ComputeTotalNumChunks(const int mipLevel) const
    {
        const Vector3i dims = MipLevelDimensionsInMipChunks(mipLevel);
        return dims.x * dims.y * dims.z;
    }

    inline uint32_t GetNumberOfVoxelsPerChunk() const
    {
        const Vector3i dims = GetChunkDimensions();
        return dims.x * dims.y * dims.z;
    }

    inline Matrix4f DataToGlobalMat(int mipLevel) const {
        Matrix4f ret = dataToGlobal_;
        int factor = om::math::pow2int(mipLevel);
        ret.scaleTranslation(1.0f / factor);
        ret.m33 = 1.0f / factor;
        return ret;
    }

    inline Matrix4f GlobalToDataMat(int mipLevel) const {
        Matrix4f ret = globalToData_;
        int factor = om::math::pow2int(mipLevel);
        ret.m33 = factor;
        return ret;
    }
};

