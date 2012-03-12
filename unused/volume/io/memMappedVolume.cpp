#include "volume/io/memMappedVolume.h"
#include "chunks/chunk.h"

namespace om {
namespace volume {

class LoadMemMapFilesVisitor : public boost::static_visitor<> {
public:
    template <typename T> void operator()( T& d) const {
        d.Load();
    }
};
void memMappedVolume::loadMemMapFiles()
{
    boost::apply_visitor(LoadMemMapFilesVisitor(), volData_);
}

class AllocMemMapFilesVisitor : public boost::static_visitor<> {
public:
    AllocMemMapFilesVisitor(const std::map<int, Vector3i>& levDims)
        : levelsAndDims(levDims) {}
    template <typename T> void operator()( T& d) const {
        d.Create(levelsAndDims);
    }
private:
    const std::map<int, Vector3i> levelsAndDims;
};
void memMappedVolume::allocMemMapFiles(const std::map<int, Vector3i>& levDims){
    boost::apply_visitor(AllocMemMapFilesVisitor(levDims), volData_);
}


class GetBytesPerVoxelVisitor : public boost::static_visitor<int> {
public:
    template <typename T> int operator()( T& d) const {
        return d.GetBytesPerVoxel();
    }
};
int memMappedVolume::GetBytesPerVoxel() const {
    return boost::apply_visitor(GetBytesPerVoxelVisitor(), volData_);
}


class GetChunkPtrVisitor : public boost::static_visitor<rawDataPtrs>{
public:
    GetChunkPtrVisitor(const coords::chunkCoord& coord)
        : coord(coord) {}

    template <typename T>
    rawDataPtrs operator()(T& d) const {
        return d.GetChunkPtr(coord);
    }
private:
    const coords::chunkCoord coord;
};
rawDataPtrs memMappedVolume::GetChunkPtr(const coords::chunkCoord& coord){
    return boost::apply_visitor(GetChunkPtrVisitor(coord), volData_);
}

}
}
