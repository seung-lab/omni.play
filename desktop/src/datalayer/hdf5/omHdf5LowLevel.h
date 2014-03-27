#pragma once
#include "precomp.h"

#ifdef WIN32
#include <windows.h>
#include <BaseTsd.h>
typedef LONG_PTR ssize_t;
#endif

#include "common/common.h"
#include "coordinates/data.h"
#include "datalayer/omDataWrapper.h"
#include "datalayer/omDataPath.h"

class OmHdf5LowLevel {
 public:
  explicit OmHdf5LowLevel(const int id);

  void setPath(const OmDataPath& p);
  const char* getPath();

  bool group_exists();
  void group_delete();
  void group_create();
  void group_create_tree(const char*);

  bool dataset_exists();
  void dataset_delete_create_tree();
  Vector3i getDatasetDims();
  OmDataWrapperPtr readDataset(int* size = nullptr);
  void allocateDataset(int size, OmDataWrapperPtr data);

  Vector3i getChunkedDatasetDims(const om::common::AffinityGraph aff);
  void allocateChunkedDataset(const Vector3i&, const Vector3i&,
                              const om::common::DataType);
  OmDataWrapperPtr readChunk(const om::coords::DataBbox&,
                             const om::common::AffinityGraph aff);
  void writeChunk(const om::coords::DataBbox&, OmDataWrapperPtr);
  OmDataWrapperPtr GetChunkDataType();

 private:
  // hid_t is typedef'd to int in H5Ipublic.h
  const int fileId;
  OmDataPath path;
};
