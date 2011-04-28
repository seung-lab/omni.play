#ifndef OM_HDF5_LOW_LEVEL_H
#define OM_HDF5_LOW_LEVEL_H

#ifdef WIN32
#include <windows.h>
#include <BaseTsd.h>
typedef LONG_PTR ssize_t;
#endif

#include "common/omCommon.h"
#include "datalayer/omDataWrapper.h"
#include "datalayer/omDataPath.h"

class OmHdf5LowLevel {
public:
    explicit OmHdf5LowLevel(const int id);

    void setPath(const OmDataPath & p);
    const char* getPath();

    bool group_exists();
    void group_delete();
    void group_create();
    void group_create_tree(const char*);

    bool dataset_exists();
    void dataset_delete_create_tree();
    Vector3i getDatasetDims();
    OmDataWrapperPtr readDataset(int* size = NULL);
    void allocateDataset(int size, OmDataWrapperPtr data);

    Vector3i getChunkedDatasetDims(const om::AffinityGraph aff);
    void allocateChunkedDataset(const Vector3i&,
                                const Vector3i&,
                                const OmVolDataType);
    OmDataWrapperPtr readChunk(const DataBbox&, const om::AffinityGraph aff);
    void writeChunk(const DataBbox&, OmDataWrapperPtr);
    OmDataWrapperPtr GetChunkDataType();

private:
    // hid_t is typedef'd to int in H5Ipublic.h
    const int fileId;
    OmDataPath path;
};
#endif
