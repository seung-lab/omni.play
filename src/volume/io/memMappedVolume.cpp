#include "volume/io/memMappedVolume.h"
#include "chunks/chunk.h"
#include "volume/build/omDownsampler.hpp"
#include "threads/omTaskManager.hpp"

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
rawDataPtrs memMappedVolume::getChunkPtrRaw(const coords::chunkCoord& coord){
    return boost::apply_visitor(GetChunkPtrVisitor(coord), volData_);
}


class GetVolPtrVisitor : public boost::static_visitor<rawDataPtrs>{
public:
    GetVolPtrVisitor(const int level) : level_(level) {}

    template <typename T>
    rawDataPtrs operator()(T& d) const {
        return d.GetPtr(level_);
    }
private:
    const int level_;
};
rawDataPtrs memMappedVolume::GetVolPtr(const int level)
{
    return boost::apply_visitor(GetVolPtrVisitor(level), volData_);
}


class GetVolPtrTypeVisitor : public boost::static_visitor<rawDataPtrs>{
public:
    template <typename T>
    rawDataPtrs operator()(T& d) const {
        return d.GetType();
    }
};
rawDataPtrs memMappedVolume::GetVolPtrType()
{
    return boost::apply_visitor(GetVolPtrTypeVisitor(), volData_);
}


class DownsampleVisitor : public boost::static_visitor<>{
public:
    DownsampleVisitor(volume* vol, volDataSrcs& volData)
        : vol_(vol)
        , volData_(volData)
    {}

    template <typename T>
    void operator()(T*) const
    {
        memMappedVolumeImpl<T>& files =
            boost::get<memMappedVolumeImpl<T> >(volData_);

        OmDownsampler<T> d(vol_, &files);
        d.DownsampleChooseOneVoxelOfNine();
    }

private:
    volume* vol_;
    volDataSrcs& volData_;
};

void memMappedVolume::downsample(volume* vol)
{
    rawDataPtrs dataType = GetVolPtrType();
    boost::apply_visitor(DownsampleVisitor(vol, volData_),
                         dataType);
}
