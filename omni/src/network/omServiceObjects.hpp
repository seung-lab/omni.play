#ifndef OM_SERVICE_OBJECTS_HPP
#define OM_SERVICE_OBJECTS_HPP

class OmAssembleTilesIntoSlice;
class OmExtractMesh;
class OmWriteTile;

class OmServiceObjects {
private:
    boost::scoped_ptr<OmAssembleTilesIntoSlice> tileAssembler_;
    boost::scoped_ptr<OmExtractMesh> meshExtractor_;
    boost::scoped_ptr<OmWriteTile> tileWriter_;

public:
    OmServiceObjects();
    ~OmServiceObjects();

    /**
     * returns UUID
     * creates
     *   /var/www/temp_omni_imgs/channel-1/UUID.png
     **/
    std::string MakeImgFileChannel(const int sliceNum);

    /**
     * returns UUID
     * creates
     *   /var/www/temp_omni_imgs/segmenation-1/UUID.png
     **/
    std::string MakeImgFileSegmentation(const int sliceNum);

    /**
     * returns UUID
     * creates
     *   /var/www/temp_omni_imgs/segmenation-1/UUID.png
     **/
    std::string SelectSegment(const OmID segmentationID, const int sliceNum,
                              const float x, float y,
                              const float w, const float h);

    std::string GetMeshData(const OmID segmentationID, const OmSegID segID,
                            const int mipLevel, const int x, const int y, const int z);

    std::string GetProjectData();
    std::string ChangeThreshold(const OmID segmentationID, const float threshold);

    std::string GetSegmentationDim();
    std::string MakeTileFileChannel(const int sliceNum, const int tileX, const int tileY);
    std::string MakeTileFileSegmentation(const int sliceNum, const int tileX, const int tileY);
};

#endif
