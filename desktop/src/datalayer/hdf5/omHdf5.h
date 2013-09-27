#pragma once

#include "common/common.h"
#include "datalayer/omDataWrapper.h"
#include "volume/omVolumeTypes.hpp"
#include "zi/omMutex.h"

class OmHdf5Impl;
class OmDataPath;

class OmHdf5 {
public:
    virtual ~OmHdf5()
    {}

    const std::string& getFileNameAndPath() const {
        return fnp_;
    }

    //file
    void open();
    void close();
    void create();
    void flush();

    //group
    bool group_exists( const OmDataPath & path );
    void group_delete( const OmDataPath & path );

    //data set
    bool dataset_exists( const OmDataPath & path );

    //image I/O
    Vector3i getChunkedDatasetDims(const OmDataPath& path,
                                   const om::common::AffinityGraph aff);
    void allocateChunkedDataset( const OmDataPath &,
                                 const Vector3i&,
                                 const Vector3i&,
                                 const OmVolDataType type);
    void dataset_image_write_trim( const OmDataPath &, const om::dataBbox&,
                                   OmDataWrapperPtr data);

    //data set raw
    OmDataWrapperPtr readDataset( const OmDataPath & path, int* size = NULL);
    void writeDataset( const OmDataPath & path, int size, const OmDataWrapperPtr data);
    OmDataWrapperPtr readChunk( const OmDataPath & path,
                                const om::dataBbox& dataExtent,
                                const om::common::AffinityGraph aff);
    void writeChunk(const OmDataPath& path, om::dataBbox dataExtent, OmDataWrapperPtr data);
    Vector3i getDatasetDims( const OmDataPath & path );
    OmDataWrapperPtr GetChunkDataType(const OmDataPath& path);

private:
    OmHdf5(const std::string& fnp, const bool readOnly)
        : fnp_(fnp)
        , readOnly_(readOnly)
    {}

    const std::string fnp_;
    const bool readOnly_;

    zi::rwmutex fileLock;
    std::shared_ptr<OmHdf5Impl> hdf5_;

    friend class OmHdf5Manager;
};

