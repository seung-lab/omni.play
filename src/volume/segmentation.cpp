#include "chunks/segChunk.h"
#include "common/common.h"
#include "common/debug.h"
#include "datalayer/dataPaths.h"
//#include "mesh/meshManagers.hpp"
#include "segment/segments.h"
#include "volume/io/volumeData.h"
#include "volume/segmentation.h"
#include "volume/segmentationFolder.h"
//#include "volume/segmentationLoader.h"

namespace om {
namespace volume {

// used by dataArchiveProject
segmentation::segmentation()
    : segments_(new segment::segments(this))
    , volData_(new data())
{}

// used by genericManager
segmentation::segmentation(common::id id)
    : common::manageableObject(id)
//    , loader_(new om::segmentation::loader(this))
//    , mesh::managers_(new mesh::managers(this))
    , segments_(new segment::segments(this))
    , volData_(new data())
{
    LoadPath();
}

void segmentation::LoadPath(){
    folder_.reset(new om::segmentation::folder(this));
}

bool segmentation::LoadVolData()
{
    if(IsBuilt())
    {
        volData_->load(this);
        return true;
    }

    return false;
}

std::string segmentation::GetName(){
    return "segmentation" + string::num(GetID());
}

std::string segmentation::GetNameHyphen(){
    return "segmentation-" + string::num(GetID());
}

bool segmentation::LoadVolDataIfFoldersExist()
{
    //assume level 0 data always present
    const std::string path = datalayer::fileNames::GetVolDataFolderPath(this, 0);

    if(file::exists(path)){
        return LoadVolData();
    }

    return false;
}

int segmentation::GetBytesPerVoxel() const {
    return volData_->GetBytesPerVoxel();
}

int segmentation::GetBytesPerSlice() const {
    return GetBytesPerVoxel()*coords_.GetChunkDimension()*coords_.GetChunkDimension();
}

void segmentation::SetVolDataType(const dataType type)
{
    mVolDataType = type;
    volData_->SetDataType(this);
}

void segmentation::Flush()
{
    folder_->MakeUserSegmentsFolder();

    segments_->Flush();
}

uint32_t segmentation::GetVoxelValue(const coords::globalCoord & vox)
{
    if(!ContainsVoxel(vox)){
        return 0;
    }

    //find mip_coord and offset
    const coords::chunkCoord mip0coord = vox.toChunkCoord(*this, 0);

    boost::shared_ptr<chunks::segChunk> chunk = GetChunk(mip0coord);

    //get voxel data
    return chunk->GetVoxelValue(vox.toDataCoord(*this, 0));
}

void segmentation::SetVoxelValue(const coords::globalCoord& vox, const uint32_t val)
{
    if (!ContainsVoxel(vox))
        return;

    for(int level = 0; level <= coords_.GetRootMipLevel(); level++)
    {
        coords::chunkCoord leaf_mip_coord = vox.toChunkCoord(*this, level);
        boost::shared_ptr<chunks::segChunk> chunk = GetChunk(leaf_mip_coord);
        chunk->SetVoxelValue(vox.toDataCoord(*this, level), val);
    }
}

boost::shared_ptr<chunks::segChunk> segmentation::GetChunk(const coords::chunkCoord& coord){
    return boost::make_shared<chunks::segChunk>(this, coord);
}

std::string segmentation::GetDirectoryPath() const {
    return folder_->RelativeVolPath();
}

} // namespace volume
} // namespace om
