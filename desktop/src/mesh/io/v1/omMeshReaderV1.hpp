#pragma once

#include "common/omDebug.h"
#include "datalayer/hdf5/omHdf5.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataPaths.h"
#include "mesh/io/omDataForMeshLoad.hpp"
#include "mesh/omMeshCoord.h"
#include "chunks/omChunk.h"
#include "volume/omSegmentation.h"
#include "project/omProject.h"

class OmMeshReaderV1{
private:
    OmSegmentation *const segmentation_;
    OmHdf5* reader_;

public:
    OmMeshReaderV1(OmSegmentation* segmentation)
        : segmentation_(segmentation)
    {
        reader_ = OmProject::OldHDF5();
    }

    bool IsAnyMeshDataPresent()
    {
        const om::coords::Chunk coord(0, 0, 0, 0);

        const ChunkUniqueValues segIDs =
            segmentation_->ChunkUniqueValues()->Values(coord, 1);

        FOR_EACH(iter, segIDs){
            if(isMeshDataPresent(*iter, coord)){
                return true;
            }
        }

        return false;
    }

    inline om::shared_ptr<OmDataForMeshLoad>
    Read(const OmMeshCoord& meshCoord){
        return Read(meshCoord.SegID(), meshCoord.Coord());
    }

    inline om::shared_ptr<OmDataForMeshLoad>
    Read(const om::common::SegID segID, const om::coords::Chunk& coord)
    {
        try{
            return doRead(segID, coord);
        } catch(...){
            return om::make_shared<OmDataForMeshLoad>();
        }
    }

private:

    bool isMeshDataPresent(const om::common::SegID segID, const om::coords::Chunk& coord)
    {
        try{
            return doIsMeshDataPresent(segID, coord);
        } catch(...){
            return false;
        }
    }

    bool doIsMeshDataPresent(const om::common::SegID segID, const om::coords::Chunk& coord)
    {
        const OmMeshCoord meshCoord(coord, segID);

        const std::string path = getDirectoryPath(meshCoord);

        OmDataPath fpath( path + "metamesh.dat" );
        if(!reader_->dataset_exists(fpath)){
            return false;
        }

        OmDataWrapperPtr result = reader_->readDataset(fpath);
        unsigned char noData = *(result->getPtr<unsigned char>());
        if ( 0 == noData ){
            return false;
        }

        return true;
    }

    om::shared_ptr<OmDataForMeshLoad>
    doRead(const om::common::SegID segID, const om::coords::Chunk& coord)
    {
        om::shared_ptr<OmDataForMeshLoad> ret =
            om::make_shared<OmDataForMeshLoad>();

        const OmMeshCoord meshCoord(coord, segID);

        const std::string path = getDirectoryPath(meshCoord);

        OmDataPath fpath( path + "metamesh.dat" );
        if( !reader_->dataset_exists( fpath ) ){
            return ret;
        }

        OmDataWrapperPtr result = reader_->readDataset(fpath);
        unsigned char noData = *(result->getPtr<unsigned char>());
        if ( 0 == noData ){
            return ret;
        }

        ret->HasData(true);

        std::pair<int, om::shared_ptr<uint32_t> > trian =
            copyOutData<uint32_t>(path, "trianoffset.dat" );
        if(trian.first){
            ret->SetTrianData(trian.second,
                              trian.first / (2 * sizeof(uint32_t)),
                              trian.first);
        }

        std::pair<int, om::shared_ptr<uint32_t> > strips =
            copyOutData<uint32_t>(path, "stripoffset.dat" );
        if(strips.first){
            ret->SetStripData(strips.second,
                              strips.first / (2 * sizeof(uint32_t)),
                              strips.first);
        }

        std::pair<int, om::shared_ptr<uint32_t> > vertexIndex =
            copyOutData<uint32_t>(path, "vertexoffset.dat" );
        if(vertexIndex.first){
            ret->SetVertexIndex(vertexIndex.second,
                                vertexIndex.first / sizeof(uint32_t),
                                vertexIndex.first);
        }

        std::pair<int, om::shared_ptr<float> > vertexData =
            copyOutData<float>(path, "vertex.dat" );
        if(vertexData.first){
            ret->SetVertexData(vertexData.second,
                               vertexData.first / (6 * sizeof(float)),
                               vertexData.first);
        }

        return ret;
    }

    std::string getDirectoryPath(const OmMeshCoord& meshCoord)
    {
        const std::string p = str( boost::format("%1%/%2%_%3%_%4%/mesh/%5%/")
                                   % meshCoord.MipChunkCoord.Level
                                   % meshCoord.MipChunkCoord.Coordinate.x
                                   % meshCoord.MipChunkCoord.Coordinate.y
                                   % meshCoord.MipChunkCoord.Coordinate.z
                                   % meshCoord.DataValue);

        return segmentation_->Folder()->RelativeVolPath().toStdString() + p;
    }

    template <typename T>
    std::pair<int, om::shared_ptr<T> >
    copyOutData(const std::string& path, const std::string& fileName)
    {
        const OmDataPath fpath( path + fileName );

        int size;
        OmDataWrapperPtr wrappedData = reader_->readDataset(fpath, &size);

        om::shared_ptr<T> data;
        if(size){
            data = om::ptrs::UnWrap<T>(wrappedData);
        }

        return std::make_pair(size, data);
    }
};

