#ifndef OM_VOL_COORDS_MIPPED_HPP
#define OM_VOL_COORDS_MIPPED_HPP

#include "volume/omVolCoords.hpp"

class OmMipVolCoords : public OmVolCoords {
private:
    int mMipLeafDim; //must be even

    //inferred from leaf dim and source data extent
    int mMipRootLevel;

    friend class OmDataArchiveProject;
    friend class OmMipVolumeArchive;
    friend class OmMipVolumeArchiveOld;

    friend QDataStream& operator<<(QDataStream& out, const OmMipVolCoords& c);
    friend QDataStream& operator>>(QDataStream& in, OmMipVolCoords& c);

public:
    OmMipVolCoords()
        : mMipLeafDim(0)
        , mMipRootLevel(0)
    {}

    virtual ~OmMipVolCoords()
    {}

//mip level method

    /**
     *  Calculate MipRootLevel using GetChunkDimension().
     */
    void UpdateRootLevel()
    {
        //determine max level
        Vector3i source_dims = GetDataExtent().getUnitDimensions();
        int max_source_dim = source_dims.getMaxComponent();
        int mipchunk_dim = GetChunkDimension();

        if (max_source_dim <= mipchunk_dim) {
            mMipRootLevel = 0;
        } else {
            mMipRootLevel = ceil(log((float) (max_source_dim) / GetChunkDimension()) / log((float)2));
        }
    }

    inline int GetRootMipLevel() const {
        return mMipRootLevel;
    }

    /**
     *  Calculate the data dimensions needed to contain the volume at a given compression level.
     */
    inline Vector3i MipLevelDataDimensions(const int level) const
    {
        //get dimensions
        DataBbox source_extent = GetDataExtent();
        Vector3f source_dims = source_extent.getUnitDimensions();

        //dims in fraction of pixels
        Vector3f mip_level_dims = source_dims / om::pow2int(level);

        return Vector3i(ceil(mip_level_dims.x),
                        ceil(mip_level_dims.y),
                        ceil(mip_level_dims.z));
    }

    /**
     *  Calculate the MipChunkCoord dims required to contain all the chunks of a given level.
     */
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

    /*
     *  Returns MipChunkCoord containing given data coordinate for given MipLevel
     */
    inline OmChunkCoord DataToMipCoord(const DataCoord& dataCoord,
                                       const int level) const
    {
        return DataToMipCoord(dataCoord, level, GetChunkDimensions());
    }

    inline static OmChunkCoord DataToMipCoord(const DataCoord& dataCoord,
                                              const int level,
                                              const Vector3i& chunkDimensions)
    {
        if( dataCoord.x < 0 ||
            dataCoord.y < 0 ||
            dataCoord.z < 0 )
        {
            return OmChunkCoord::NULL_COORD;
        }

        const int factor = om::pow2int(level);
        return OmChunkCoord(level,
                            dataCoord.x / factor / chunkDimensions.x,
                            dataCoord.y / factor / chunkDimensions.y,
                            dataCoord.z / factor / chunkDimensions.z);
    }

    inline OmChunkCoord NormToMipCoord(const NormCoord &normCoord,
                                       const int level) const
    {
        return DataToMipCoord(NormToDataCoord(normCoord), level);
    }

    /**
     *  Returns the extent of the data in specified level covered by the given MipCoordinate.
     */
    inline DataBbox MipCoordToDataBbox(const OmChunkCoord & rMipCoord,
                                       const int newLevel) const
    {

        int old_level_factor = om::pow2int(rMipCoord.Level);
        int new_level_factor = om::pow2int(newLevel);

        //convert to leaf level dimensions
        int leaf_dim = GetChunkDimension() * old_level_factor;
        Vector3i leaf_dims = GetChunkDimensions() * old_level_factor;

        //min of extent in leaf data coordinates
        DataCoord leaf_min_coord = rMipCoord.Coordinate * leaf_dim;

        //convert to new level
        DataCoord new_extent_min_coord = leaf_min_coord / new_level_factor;

        Vector3i new_dims = leaf_dims / new_level_factor;

        //return
        return DataBbox(new_extent_min_coord, new_dims.x, new_dims.y, new_dims.z);
    }

    inline NormBbox MipCoordToNormBbox(const OmChunkCoord & rMipCoord) const {
        return DataToNormBbox(MipCoordToDataBbox(rMipCoord, 0));
    }

    inline DataBbox DataToDataBBox(const DataCoord &vox, const int level) const {
        return MipCoordToDataBbox(DataToMipCoord(vox, level), level);
    }

    //mip chunk methods
    inline OmChunkCoord RootMipChunkCoordinate() const {
        return OmChunkCoord(mMipRootLevel, 0, 0, 0);
    }

    /*
     *  Returns true if given MipCoordinate is a valid coordinate within the MipVolume.
     */
    inline bool ContainsMipChunkCoord(const OmChunkCoord & rMipCoord) const
    {
        //if level is greater than root level
        if(rMipCoord.Level < 0 ||
           rMipCoord.Level > GetRootMipLevel()){
            return false;
        }

        //convert to data box in leaf (MIP 0)
        DataBbox bbox = MipCoordToDataBbox(rMipCoord, 0);

        bbox.intersect(GetDataExtent());
        if(bbox.isEmpty()){
            return false;
        }

        //else valid mip coord
        return true;
    }

    /**
     *  Finds set of children coordinates that are valid for this MipVolume.
     */
    inline void ValidMipChunkCoordChildren(const OmChunkCoord & rMipCoord,
                                           std::set<OmChunkCoord>& children) const
    {
        //clear set
        children.clear();

        //get all possible children
        OmChunkCoord possible_children[8];
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

        return Vector3i(ROUNDUP(data_dims.x, GetChunkDimension()),
                        ROUNDUP(data_dims.y, GetChunkDimension()),
                        ROUNDUP(data_dims.z, GetChunkDimension()));
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

    void addChunkCoordsForLevel(const int mipLevel,
                                std::deque<OmChunkCoord>* coords) const
    {
        const Vector3i dims = MipLevelDimensionsInMipChunks(mipLevel);

        for (int z = 0; z < dims.z; ++z){
            for (int y = 0; y < dims.y; ++y){
                for (int x = 0; x < dims.x; ++x){
                    coords->push_back(OmChunkCoord(mipLevel, x, y, z));
                }
            }
        }
    }
};

#endif
