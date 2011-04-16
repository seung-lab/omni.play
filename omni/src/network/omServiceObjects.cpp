#include "network/omWriteTile.hpp"
#include "network/omAssembleTilesIntoSlice.hpp"
#include "network/omExtractMesh.h"
#include "network/omServiceObjects.hpp"
#include "segment/omSegmentSelected.hpp"
#include "json_spirit.h"

OmServiceObjects::OmServiceObjects()
    : tileAssembler_(new OmAssembleTilesIntoSlice())
    , meshExtractor_(new OmExtractMesh())
    , tileWriter_(new OmWriteTile())
{}

OmServiceObjects::~OmServiceObjects()
{}

std::string OmServiceObjects::MakeImgFileChannel(const int sliceNum){
    return tileAssembler_->MakeImgFileChannel(sliceNum);
}

std::string OmServiceObjects::MakeImgFileSegmentation(const int sliceNum){
    return tileAssembler_->MakeImgFileSegmentation(sliceNum);
}

std::string OmServiceObjects::SelectSegment(const OmID segmentationID, const int sliceNum,
                                            const float x, float y,
                                            const float w, const float h)
{
    SegmentationDataWrapper sdw(segmentationID);
    OmSegmentation& vol = sdw.GetSegmentation();

    const AxisAlignedBoundingBox<int> volExtent = vol.Coords().GetDataExtent();

    const DataCoord coord(x * volExtent.getMax().x / w,
                          y * volExtent.getMax().y / h,
                          sliceNum);

    std::cout << "coord: " << coord << "\n";

    if(volExtent.contains(coord))
    {
        OmSegment* seg = vol.Segments()->findRoot(vol.GetVoxelValue(coord));

        if(seg)
        {
            seg->SetSelected(!seg->IsSelected(), true);
        }
    }

    return tileAssembler_->MakeImgFileSegmentation(sliceNum);
}

std::string OmServiceObjects::GetMeshData(const OmID segmentationID, const OmSegID segID,
                                          const int mipLevel, const int x, const int y, const int z)

{
    return meshExtractor_->WriteOutMesh(segmentationID, segID, mipLevel, x, y, z);
}

std::string OmServiceObjects::GetProjectData()
{
    json_spirit::Object obj;
    obj.push_back(json_spirit::Pair("fileName", OmProject::OmniFile().toStdString()));

    json_spirit::Array array;
    array.push_back(obj);

    return json_spirit::write_formatted(array);
}

std::string OmServiceObjects::ChangeThreshold(const OmID segmentationID, const float threshold)
{
    SegmentationDataWrapper sdw(segmentationID);
    if(!sdw.IsValidWrapper()){
        return "invalid segmentationID";
    }

    OmSegmentation& vol = sdw.GetSegmentation();

    vol.SetDendThreshold( threshold );

    return "threshold changed to " + om::string::num(threshold);
}

std::string OmServiceObjects::GetSegmentationDim()
{
    SegmentationDataWrapper sdw(1);
    if(!sdw.IsValidWrapper()){
        return "invalid segmentationID";
    }

    OmSegmentation& vol = sdw.GetSegmentation();

    const Vector3i dims = vol.Coords().MipLevelDimensionsInMipChunks(0);
    const int rootMipLevel = vol.Coords().GetRootMipLevel();

    json_spirit::Array array;

    for(int mipLevel = 0; mipLevel <= rootMipLevel; ++mipLevel)
    {
        const Vector3i dims = vol.Coords().MipLevelDimensionsInMipChunks(mipLevel);

        json_spirit::Object obj;

        obj.push_back( json_spirit::Pair("mip_level", mipLevel) );
        obj.push_back( json_spirit::Pair("num_cols", dims.x) );
        obj.push_back( json_spirit::Pair("num_rows", dims.y) );
        obj.push_back( json_spirit::Pair("num_slices", dims.z) );
        obj.push_back( json_spirit::Pair("tile_width", 128) );

        array.push_back(obj);
    }

    return json_spirit::write_formatted(array);
}

std::string OmServiceObjects::MakeTileFileChannel(const int sliceNum,
                                                  const int tileX, const int tileY)
{
    const DataCoord dataCoord(tileX * 128, tileY * 128, sliceNum);
    return tileWriter_->MakeTileFileChannel(dataCoord);
}

std::string OmServiceObjects::MakeTileFileSegmentation(const int sliceNum,
                                                       const int tileX, const int tileY)
{
    const DataCoord dataCoord(tileX * 128, tileY * 128, sliceNum);
    return tileWriter_->MakeTileFileSegmentation(dataCoord);
}

