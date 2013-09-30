#include "common/logging.h"
#include "datalayer/hdf5/omHdf5LowLevel.h"
#include "datalayer/hdf5/omHdf5Utils.hpp"
#include "datalayer/omDataPaths.h"
#include "utility/omSystemInformation.h"

#include <QStringList>
#include "hdf5.h"

OmHdf5LowLevel::OmHdf5LowLevel(const int id) : fileId(id) {}

void OmHdf5LowLevel::setPath(const OmDataPath& p) { path = p; }

const char* OmHdf5LowLevel::getPath() { return path.getString().c_str(); }

bool OmHdf5LowLevel::group_exists() {
  return OmHdf5Utils::group_exists(fileId, getPath());
}

void OmHdf5LowLevel::group_delete() {
  OmHdf5Utils::group_delete(fileId, getPath());
}

void OmHdf5LowLevel::group_create() {
  OmHdf5Utils::group_create(fileId, getPath());
}

bool OmHdf5LowLevel::dataset_exists() {
  return OmHdf5Utils::dataset_exists(fileId, getPath());
}

/**
 * method used to read meshes and .dat files from disk
 */
OmDataWrapperPtr OmHdf5LowLevel::readDataset(int* size) {
  //debug(hdf5verbose, "in %s: path is %s\n", __FUNCTION__, getPath());

  if (!OmHdf5Utils::group_exists(fileId, getPath())) {
    *size = 0;
    return OmDataWrapperInvalid();
  }

  herr_t status;

  //Opens an existing dataset.
  hid_t dataset_id = H5Dopen2(fileId, getPath(), H5P_DEFAULT);
  if (dataset_id < 0) {
    const std::string errMsg =
        "Could not open HDF5 dataset (even though it existed)" +
        std::string(getPath());
    throw om::IoException(errMsg);
  }

  //Returns an identifier for a copy of the dataspace for a dataset.
  hid_t dataspace_id = H5Dget_space(dataset_id);
  if (dataspace_id < 0) {
    throw om::IoException("Could not get HDF5 dataspace.");
  }

  //Returns the amount of storage required for a dataset.
  hsize_t dataset_size = H5Dget_storage_size(dataset_id);
  if (NULL != size) {
    *size = (int) dataset_size;
  }

  void* dataset_data = malloc(dataset_size);
  if (NULL == dataset_data) {
    throw om::IoException("Could not allocate memory to read HDF5 dataset.");
  }

  hid_t dstype = H5Dget_type(dataset_id);
  //Reads raw data from a dataset into a buffer.
  status =
      H5Dread(dataset_id, dstype, H5S_ALL, H5S_ALL, H5P_DEFAULT, dataset_data);
  if (status < 0) {
    const std::string errMsg =
        "Could not open HDF5 dataset " + std::string(getPath());
    throw om::IoException(errMsg);
  }

  OmDataWrapperPtr data =
      OmHdf5Utils::getDataWrapper(dataset_data, dstype, om::MALLOC);
  H5Tclose(dstype);

  //Releases and terminates access to a dataspace.
  status = H5Sclose(dataspace_id);
  if (status < 0) {
    throw om::IoException("Could not close HDF5 dataspace.");
  }

  //Closes the specified dataset.
  status = H5Dclose(dataset_id);
  if (status < 0) {
    throw om::IoException("Could not close HDF5 dataset.");
  }

  return data;
}

void OmHdf5LowLevel::allocateDataset(int size, OmDataWrapperPtr data) {
  //debug(hdf5verbose, "OmHDF5LowLevel: in %s...\n", __FUNCTION__);

  herr_t status;

  //Creates a new simple dataspace and opens it for access.
  int rank = 1;
  hsize_t dim = size / data->getSizeof();

  // TODO: fixme! use the max dim!
  hsize_t max = dim;
  hid_t dataspace_id = H5Screate_simple(rank, &dim, &max);
  if (dataspace_id < 0) {
    throw om::IoException("Could not create HDF5 dataspace.");
  }

  //Creates a dataset at the specified location.
  hid_t dataset_id =
      H5Dcreate2(fileId, getPath(), data->getHdf5FileType(), dataspace_id,
                 H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
  if (dataset_id < 0) {
    throw om::IoException("Could not create HDF5 dataset " +
                          std::string(getPath()));
  }

  //if given data, then write it into new dataset
  if (NULL != data->getVoidPtr()) {
    status = H5Dwrite(dataset_id, data->getHdf5FileType(), H5S_ALL, H5S_ALL,
                      H5P_DEFAULT, data->getVoidPtr());
    if (status < 0)
      throw om::IoException("Could not copy data into HDF5 dataset.");
  }

  //Closes the specified dataset.
  status = H5Dclose(dataset_id);
  if (status < 0) {
    throw om::IoException("Could not close HDF5 dataset.");
  }

  //Releases and terminates access to a dataspace.
  status = H5Sclose(dataspace_id);
  if (status < 0) {
    throw om::IoException("Could not close HDF5 dataspace.");
  }
}

/**
 *  Creates nested group tree.  Ignores already existing groups.
 */
void OmHdf5LowLevel::group_create_tree(const char* path) {
  ////debug(hdf5verbose, "OmHDF5LowLevel: in %s...\n", __FUNCTION__);

  std::string curPath;
  const QStringList splits = QString(path).split('/');
  FOR_EACH(iter, splits) {
    curPath += iter->toStdString() + "/";

    //create if group does not exist
    if (!OmHdf5Utils::group_exists(fileId, curPath.c_str())) {
      OmHdf5Utils::group_create(fileId, curPath.c_str());
    }
  }
}

/////////////////////////////////
///////          Dataset private

void OmHdf5LowLevel::dataset_delete_create_tree() {
  //debug(hdf5verbose, "OmHDF5LowLevel: in %s...\n", __FUNCTION__);

  //get position of last slash
  std::string name_str(getPath());
  size_t pos_last_slash = name_str.find_last_of(std::string("/"));

  //if there was a slash
  if ((std::string::npos != pos_last_slash) && (pos_last_slash > 0)) {
    //split into group path and name
    std::string group_path(name_str, 0, pos_last_slash);

    group_create_tree(group_path.c_str());
  }

  OmHdf5Utils::dataset_delete_if_exists(fileId, getPath());
}

Vector3i OmHdf5LowLevel::getChunkedDatasetDims(
    const om::common::AffinityGraph affin) {
  om::common::AffinityGraph aff = affin;
  //debug(hdf5verbose, "OmHDF5LowLevel: in %s...\n", __FUNCTION__);

  herr_t status;

  //Opens an existing dataset.
  hid_t dataset_id = H5Dopen2(fileId, getPath(), H5P_DEFAULT);
  if (dataset_id < 0) {
    throw om::IoException("Could not open HDF5 dataset " +
                          std::string(getPath()));
  }

  //Returns an identifier for a copy of the dataspace for a dataset.
  hid_t dataspace_id = H5Dget_space(dataset_id);
  if (dataspace_id < 0) {
    throw om::IoException("Could not get HDF5 dataspace.");
  }

  //Determines the dimensionality of a dataspace.
  int rank = H5Sget_simple_extent_ndims(dataspace_id);
  if (rank < 0) {
    throw om::IoException("Could not determine rank of HDF5 dataspace.");
  }

  if (4 == rank && aff == om::common::AffinityGraph::NO_AFFINITY) {
    aff = om::common::AffinityGraph::X_AFFINITY;
    //throw om::IoException("HDF5 dataspace rank 4 but doesn't have a set
    //affinity.");
  }

  if (4 == rank) {
    //Retrieves dataspace dimension size and maximum size.
    Vector3<hsize_t> dims;
    Vector3<hsize_t> maxdims;
    Vector4<hsize_t> dims4;
    Vector4<hsize_t> maxdims4;
    rank = H5Sget_simple_extent_dims(dataspace_id, dims4.array, maxdims4.array);
    if (rank < 0) {
      throw om::IoException(
          "Could not determine dimensions of HDF5 dataspace.");
    }

    //Releases and terminates access to a dataspace.
    status = H5Sclose(dataspace_id);
    if (status < 0) {
      throw om::IoException("Could not close HDF5 dataspace.");
    }

    //Closes the specified dataset.
    status = H5Dclose(dataset_id);
    if (status < 0) {
      throw om::IoException("Could not close HDF5 dataset.");
    }

    //debug(hdf5image, "dims are %d:%d:%d; maxdims are %d:%d:%d\n",
    ////DEBUGV3(dims), //DEBUGV3(maxdims));
    //printf("dims4 are %i:%i:%i:%i\n", dims4.array[0], dims4.array[1],
    //dims4.array[2], dims4.array[3]);

    //flip from hdf5 version
    return Vector3i(dims4.array[3], dims4.array[2], dims4.array[1]);
  } else if (3 == rank) {
    //Retrieves dataspace dimension size and maximum size.
    Vector3<hsize_t> dims;
    Vector3<hsize_t> maxdims;
    rank = H5Sget_simple_extent_dims(dataspace_id, dims.array, maxdims.array);
    if (rank < 0) {
      throw om::IoException(
          "Could not determine dimensions of HDF5 dataspace.");
    }

    //Releases and terminates access to a dataspace.
    status = H5Sclose(dataspace_id);
    if (status < 0) {
      throw om::IoException("Could not close HDF5 dataspace.");
    }

    //Closes the specified dataset.
    status = H5Dclose(dataset_id);
    if (status < 0) {
      throw om::IoException("Could not close HDF5 dataset.");
    }

    //debug(hdf5image, "dims are %d:%d:%d; maxdims are %d:%d:%d\n",
    //DEBUGV3(dims), //DEBUGV3(maxdims));

    //flip from hdf5 version
    return Vector3i(dims.z, dims.y, dims.x);
  }

  assert(0 && "rank isn't 3 or 4...");
  return Vector3i(0, 0, 0);
}

void OmHdf5LowLevel::allocateChunkedDataset(const Vector3i& dataDims,
                                            const Vector3i& chunkDims,
                                            const OmVolDataType type) {
  herr_t ret;
  const int rank = 3;
  //debug(hdf5verbose, "OmHDF5LowLevel: in %s...i\n", __FUNCTION__);

  //Creates a new property as an instance of a property list class.
  hid_t plist_id = H5Pcreate(H5P_DATASET_CREATE);
  if (plist_id < 0) {
    throw om::IoException("Could not create HDF5 property list.");
  }

  //Sets the size of the chunks used to store a chunked layout dataset.
  Vector3<hsize_t> flipped_chunk_dim(chunkDims.z, chunkDims.y, chunkDims.x);
  ret = H5Pset_chunk(plist_id, rank, flipped_chunk_dim.array);
  if (ret < 0) {
    throw om::IoException("Could not set HDF5 chunk size.");
  }
  hid_t whenAlloc = H5Pset_alloc_time(plist_id, H5D_ALLOC_TIME_EARLY);
  if (whenAlloc < 0) {
    throw om::IoException("Could not set HDF5 alloc time.");
  }

  //data dims
  Vector3<hsize_t> flipped_data_dims(dataDims.z, dataDims.y, dataDims.x);
  Vector3<hsize_t> flipped_max_data_dims;

  flipped_max_data_dims = Vector3<hsize_t>(dataDims.z, dataDims.y, dataDims.x);

  //Creates a new simple dataspace and opens it for access.
  hid_t dataspace_id = H5Screate_simple(rank, flipped_data_dims.array,
                                        flipped_max_data_dims.array);
  if (dataspace_id < 0) {
    throw om::IoException("Could not create HDF5 dataspace.");
  }

  //Creates a dataset at the specified location.
  hid_t type_id = OmVolumeTypeHelpers::getHDF5FileType(type);
  hid_t dataset_id = H5Dcreate2(fileId, getPath(), type_id, dataspace_id,
                                H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
  if (dataset_id < 0) {
    throw om::IoException("Could not create HDF5 dataset " +
                          std::string(getPath()));
  }

  //Terminates access to a property list.
  ret = H5Pclose(plist_id);
  if (ret < 0) {
    throw om::IoException("Could not close HDF5 property list.");
  }

  //Releases and terminates access to a dataspace.
  ret = H5Sclose(dataspace_id);
  if (ret < 0) {
    throw om::IoException("Could not close HDF5 dataspace.");
  }

  //Closes the specified dataset.
  ret = H5Dclose(dataset_id);
  if (ret < 0) {
    throw om::IoException("Could not close HDF5 dataset.");
  }
}

void OmHdf5LowLevel::writeChunk(const om::dataBbox& extent,
                                OmDataWrapperPtr data) {
  //debug(hdf5verbose, "OmHDF5LowLevel: in %s...\n", __FUNCTION__);

  //Opens an existing dataset.
  hid_t dataset_id = H5Dopen2(fileId, getPath(), H5P_DEFAULT);
  if (dataset_id < 0) {
    throw om::IoException("Could not open HDF5 dataset " +
                          std::string(getPath()));
  }

  //Returns an identifier for a copy of the dataspace for a dataset.
  hid_t dataspace_id = H5Dget_space(dataset_id);
  if (dataspace_id < 0) {
    throw om::IoException("Could not get HDF5 dataspace.");
  }

  //create start, stride, count, block
  //flip coordinates cuz thats how hdf5 likes it
  Vector3<hsize_t> start = extent.getMin();
  Vector3<hsize_t> end = extent.getMax();
  Vector3<hsize_t> start_flipped(start.z, start.y, start.x);

  Vector3<hsize_t> stride = Vector3i::ONE;
  Vector3<hsize_t> count = Vector3i::ONE;

  Vector3<hsize_t> block = extent.getUnitDimensions();
  Vector3<hsize_t> block_flipped(block.z, block.y, block.x);

  //Selects a hyperslab region to add to the current selected region.
  herr_t ret =
      H5Sselect_hyperslab(dataspace_id, H5S_SELECT_SET, start_flipped.array,
                          stride.array, count.array, block_flipped.array);
  if (ret < 0) throw om::IoException("Could not select HDF5 hyperslab.");

  //Creates a new simple dataspace and opens it for access.
  hid_t mem_dataspace_id = H5Screate_simple(3, block.array, block.array);
  if (mem_dataspace_id < 0)
    throw om::IoException(
        "Could not create scratch HDF5 dataspace to read data into.");

  //setup image data
  Vector3i extent_dims = extent.getUnitDimensions();

  //Reads raw data from a dataset into a buffer.
  void* rawData = data->getVoidPtr();
  const hid_t type = data->getHdf5MemoryType();
  ret = H5Dwrite(dataset_id, type, mem_dataspace_id, dataspace_id, H5P_DEFAULT,
                 rawData);
  if (ret < 0) {
    throw om::IoException("Could not read HDF5 dataset " +
                          std::string(getPath()));
  }

  //Releases and terminates access to a dataspace.
  //herr_t H5Sclose(hid_t space_id  )
  ret = H5Sclose(mem_dataspace_id);
  if (ret < 0) throw om::IoException("Could not close HDF5 scratch dataspace.");

  ret = H5Sclose(dataspace_id);
  if (ret < 0) throw om::IoException("Could not close HDF5 dataspace.");

  //Closes the specified dataset.
  //herr_t H5Dclose(hid_t dataset_id  )
  ret = H5Dclose(dataset_id);
  if (ret < 0) throw om::IoException("Could not close HDF5 dataset.");
}

OmDataWrapperPtr OmHdf5LowLevel::GetChunkDataType() {
  //Opens an existing dataset.
  //hid_t H5Dopen(hid_t loc_id, const char *name  )
  hid_t dataset_id = H5Dopen2(fileId, getPath(), H5P_DEFAULT);
  if (dataset_id < 0) {
    throw om::IoException("Could not open HDF5 dataset " +
                          std::string(getPath()));
  }

  hid_t dstype = H5Dget_type(dataset_id);

  OmDataWrapperPtr data = OmHdf5Utils::getDataWrapper(NULL, dstype, om::NONE);

  H5Tclose(dstype);

  int ret = H5Dclose(dataset_id);
  if (ret < 0) {
    throw om::IoException("Could not close HDF5 dataset.");
  }

  return data;
}

OmDataWrapperPtr OmHdf5LowLevel::readChunk(
    const om::dataBbox& extent, const om::common::AffinityGraph aff) {
  //debug(hdf5verbose, "OmHDF5LowLevel: in %s...\n", __FUNCTION__);

  //Opens an existing dataset.
  //hid_t H5Dopen(hid_t loc_id, const char *name  )
  hid_t dataset_id = H5Dopen2(fileId, getPath(), H5P_DEFAULT);
  if (dataset_id < 0)
    throw om::IoException("Could not open HDF5 dataset " +
                          std::string(getPath()));

  hid_t dstype = H5Dget_type(dataset_id);
  //printTypeInfo( dstype );

  //Returns an identifier for a copy of the dataspace for a dataset.
  //hid_t H5Dget_space(hid_t dataset_id  )
  hid_t dataspace_id = H5Dget_space(dataset_id);
  if (dataspace_id < 0) throw om::IoException("Could not get HDF5 dataspace.");

  //create start, stride, count, block
  //flip coordinates cuz thats how hdf5 likes it
  Vector3<hsize_t> start = extent.getMin();
  Vector3<hsize_t> end = extent.getMax();
  Vector3<hsize_t> start_flipped(start.z, start.y, start.x);

  Vector3<hsize_t> stride = Vector3i::ONE;
  Vector3<hsize_t> count = Vector3i::ONE;

  Vector3<hsize_t> block = extent.getDimensions();
  Vector3<hsize_t> block_flipped(block.z, block.y, block.x);
  //debug(hdf5image, "start:%i,%i,%i\n", //DEBUGV3(start));
  //debug(hdf5image, "block:%i,%i,%i\n", //DEBUGV3(block));
  herr_t ret;
  hid_t mem_dataspace_id;
  OmDataWrapperPtr data;

  int rank = H5Sget_simple_extent_ndims(dataspace_id);

  if (4 == rank && om::common::AffinityGraph::NO_AFFINITY !=
                       aff) {  // Load a side of the affinity graph.
    //Selects a hyperslab region to add to the current selected region.

    const Vector4<hsize_t> start_flipped(0, start.z, start.y, start.x);
    const Vector4<hsize_t> stride(1, 1, 1, 1);
    const Vector4<hsize_t> count(1, 1, 1, 1);

    const Vector4<hsize_t> block_flipped(3, block.z, block.y, block.x);

    ret = H5Sselect_hyperslab(dataspace_id, H5S_SELECT_SET, start_flipped.array,
                              stride.array, count.array, block_flipped.array);
    if (ret < 0) {
      throw om::IoException("Could not select HDF5 hyperslab.");
    }

    const Vector4<hsize_t> block4(extent.getDimensions().x,
                                  extent.getDimensions().y,
                                  extent.getDimensions().z, 3);

    //Creates a new simple dataspace and opens it for access.
    mem_dataspace_id = H5Screate_simple(4, block4.array, NULL);
    if (mem_dataspace_id < 0) {
      throw om::IoException(
          "Could not create scratch HDF5 dataspace to read data into.");
    }

    //setup image data
    const Vector3i extent_dims = extent.getDimensions();
    const int64_t size = extent_dims.x * extent_dims.y * extent_dims.z *
                         OmHdf5Utils::getSizeofType(dstype) * 3;

    if (size < 1) {
      throw om::IoException("invalid size");
    }

    char* imageData = static_cast<char*>(malloc(size));
    if (NULL == imageData) {
      throw om::IoException("Could not allocate memory to read HDF5 dataset.");
    }

    char* imageData3 = static_cast<char*>(malloc(size / 3));
    data = OmHdf5Utils::getDataWrapper(imageData3, dstype, om::MALLOC);
    H5Tclose(dstype);

    ret = H5Dread(dataset_id, data->getHdf5MemoryType(), mem_dataspace_id,
                  dataspace_id, H5P_DEFAULT, imageData);
    if (ret < 0) {
      std::cout << "ERROR: extexts were " << extent << "\n";
      throw om::IoException("Error while reading HDF5 dataset \"" +
                            std::string(getPath()) + "\"");
    }

    printf("aff = %i\n", aff);
    memcpy(imageData3, &imageData[size * ((int) aff - 1) / 3], size / 3);

    free(imageData);
  } else {
    //Selects a hyperslab region to add to the current selected region.
    ret = H5Sselect_hyperslab(dataspace_id, H5S_SELECT_SET, start_flipped.array,
                              stride.array, count.array, block_flipped.array);
    if (ret < 0) throw om::IoException("Could not select HDF5 hyperslab.");

    //Creates a new simple dataspace and opens it for access.
    std::cout << "\tBlock: " << block << std::endl;
    mem_dataspace_id = H5Screate_simple(3, block.array, NULL);
    if (mem_dataspace_id < 0) {
      throw om::IoException(
          "Could not create scratch HDF5 dataspace to read data into.");
    }

    const Vector3i extent_dims = extent.getDimensions();
    const int64_t size = extent_dims.x * extent_dims.y * extent_dims.z *
                         OmHdf5Utils::getSizeofType(dstype);
    if (size < 1) {
      throw om::IoException("invalid size");
    }
    void* imageData = malloc(size);
    if (NULL == imageData) {
      throw om::IoException("Could not allocate memory to read HDF5 dataset.");
    }

    data = OmHdf5Utils::getDataWrapper(imageData, dstype, om::MALLOC);

    H5Tclose(dstype);

    ret = H5Dread(dataset_id, data->getHdf5MemoryType(), mem_dataspace_id,
                  dataspace_id, H5P_DEFAULT, imageData);
    if (ret < 0) {
      std::cout << "ERROR: extexts were " << extent << "\n";
      throw om::IoException("Error while reading HDF5 dataset \"" +
                            std::string(getPath()) + "\"");
    }
  }

  //Releases and terminates access to a dataspace.
  //herr_t H5Sclose(hid_t space_id  )
  ret = H5Sclose(mem_dataspace_id);
  if (ret < 0) throw om::IoException("Could not close HDF5 scratch dataspace.");

  ret = H5Sclose(dataspace_id);
  if (ret < 0) throw om::IoException("Could not close HDF5 dataspace.");

  //Closes the specified dataset.
  //herr_t H5Dclose(hid_t dataset_id  )
  ret = H5Dclose(dataset_id);
  if (ret < 0) throw om::IoException("Could not close HDF5 dataset.");

  return data;
}

Vector3i OmHdf5LowLevel::getDatasetDims() {
  //debug(hdf5verbose, "OmHDF5LowLevel: in %s...\n", __FUNCTION__);

  Vector3<hsize_t> dims(0, 0, 0);

  herr_t status;

  //Opens an existing dataset.
  hid_t dataset_id = H5Dopen2(fileId, getPath(), H5P_DEFAULT);
  if (dataset_id < 0) {
    throw om::IoException("Could not open HDF5 dataset " +
                          std::string(getPath()));
  }

  //Returns an identifier for a copy of the dataspace for a dataset.
  hid_t dataspace_id = H5Dget_space(dataset_id);
  if (dataspace_id < 0) throw om::IoException("Could not get HDF5 dataspace.");

  //Determines the dimensionality of a dataspace.
  int rank = H5Sget_simple_extent_ndims(dataspace_id);
  if (rank < 0)
    throw om::IoException("Could not determine rank of HDF5 dataspace.");

  //Retrieves dataspace dimension size and maximum size.
  Vector3<hsize_t> maxdims;
  rank = H5Sget_simple_extent_dims(dataspace_id, dims.array, maxdims.array);
  if (rank < 0)
    throw om::IoException("Could not determine dimensions of HDF5 dataspace.");

  //Releases and terminates access to a dataspace.
  status = H5Sclose(dataspace_id);
  if (status < 0) throw om::IoException("Could not close HDF5 dataspace.");

  //Closes the specified dataset.
  status = H5Dclose(dataset_id);
  if (status < 0) throw om::IoException("Could not close HDF5 dataset.");

  //flip from hdf5 version
  return dims;
}
