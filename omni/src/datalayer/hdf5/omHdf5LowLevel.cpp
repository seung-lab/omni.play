#include "common/omDebug.h"
#include "common/omException.h"
#include "common/omVtk.h"
#include "datalayer/hdf5/omHdf5LowLevel.h"
#include "datalayer/omDataPaths.h"
#include "utility/omImageDataIo.h"
#include "utility/omSystemInformation.h"

#include <QStringList>

bool OmHdf5LowLevel::group_exists(){
	return group_exists(getPath());
}

void OmHdf5LowLevel::group_delete(){
	group_delete(getPath());
}

void OmHdf5LowLevel::group_create(){
	group_create(getPath());
}

bool OmHdf5LowLevel::group_exists(const char* path)
{
        H5E_BEGIN_TRY {
 		herr_t ret = H5Gget_objinfo(fileId, path, 0, NULL);
 		if( ret < 0 ){
 			return false;
 		}
 	} H5E_END_TRY

 	return true;
}

/**
 *  Trims the read to data within the extent of the dataset. (0 fills)
 */
OmDataWrapperPtr OmHdf5LowLevel::readChunkNotOnBoundary(DataBbox dataExtent)
{
        debug("hdf5verbose", "OmHDF5LowLevel: in %s...\n", __FUNCTION__);

        const Vector3i dims = dataset_image_get_dims();

        const DataBbox dataset_extent =
		DataBbox(Vector3i::ZERO, dims.x, dims.y, dims.z);

        //if data extent contains given extent, just read from data
        if (dataset_extent.contains(dataExtent)) {
                return readChunkVTK( dataExtent);
        }
        //intersect with given extent
        DataBbox intersect_extent = dataset_extent;
        intersect_extent.intersect(dataExtent);

        if (intersect_extent.isEmpty()) {
		throw OmIoException("should not have happened");
        }

        //merge intersection and read data
        //read intersection from source
        OmDataWrapperPtr intersect_image_data =
		readChunkVTK( intersect_extent);

        //create blanks data
        OmDataWrapperPtr filled_read_data =
		OmImageDataIo::createBlankImageData(dataExtent.getUnitDimensions(),
						    intersect_image_data);

        //copy intersected data to proper location in blank image data
        //normalize to min of intersection
        DataBbox intersect_norm_intersect_extent = intersect_extent;
        intersect_norm_intersect_extent.offset(-intersect_extent.getMin());

        //normalize to min of dataExtent
        DataBbox dataextent_norm_intersect_extent = intersect_extent;
        dataextent_norm_intersect_extent.offset(-dataExtent.getMin());

        //copy data to intersect region in dataExtent from extent of intersection
        OmImageDataIo::copyImageData(filled_read_data,
				     dataextent_norm_intersect_extent,
				     intersect_image_data,
				     intersect_norm_intersect_extent);


        return filled_read_data;
}

/**
 * method used to read meshes and .dat files from disk
 */
OmDataWrapperPtr OmHdf5LowLevel::readDataset(int *size)
{
	debug("hdf5verbose", "\nOmHDF5LowLevel: in %s...\n", __FUNCTION__);
	debug("hdf5verbose", "OmHDF5LowLevel: in %s: path is %s\n",
	      __FUNCTION__, getPath());

	if(!group_exists()){
		*size = 0;
		return OmDataWrapperInvalid();
	}

	void *dataset_data;
	herr_t status;

	//Opens an existing dataset.
	hid_t dataset_id = H5Dopen2(fileId, getPath(), H5P_DEFAULT);
	if (dataset_id < 0) {
		const string errMsg = "Could not open HDF5 dataset (even though it existed)" + string(getPath());
		throw OmIoException(errMsg);
	}

	//Returns an identifier for a copy of the dataspace for a dataset.
	hid_t dataspace_id = H5Dget_space(dataset_id);
	if (dataspace_id < 0) {
		throw OmIoException("Could not get HDF5 dataspace.");
	}

	//Returns the amount of storage required for a dataset.
	hsize_t dataset_size = H5Dget_storage_size(dataset_id);
	if (NULL != size) {
		*size = (int)dataset_size;
	}

	hid_t dstype = H5Dget_type(dataset_id);

	//printTypeInfo( dstype );

	debug("hdf5verbose", "OmHDF5LowLevel: in %s: dataset size is %d\n", __FUNCTION__, (int)dataset_size);

	//Allocate memory to read into
	dataset_data = malloc(dataset_size);
	if (NULL == dataset_data) {
		throw OmIoException("Could not allocate memory to read HDF5 dataset.");
	}

	//Reads raw data from a dataset into a buffer.
	status = H5Dread(dataset_id, dstype, H5S_ALL, H5S_ALL, H5P_DEFAULT, dataset_data);
	if (status < 0) {
		const string errMsg = "Could not open HDF5 dataset " + string(getPath());
		throw OmIoException(errMsg);
	}

	OmDataWrapperPtr data = getDataWrapper(dataset_data, dstype, MALLOC);
	H5Tclose( dstype );

	//Releases and terminates access to a dataspace.
	status = H5Sclose(dataspace_id);
	if (status < 0) {
		throw OmIoException("Could not close HDF5 dataspace.");
	}

	//Closes the specified dataset.
	status = H5Dclose(dataset_id);
	if (status < 0) {
		throw OmIoException("Could not close HDF5 dataset.");
	}

	return data;
}

void OmHdf5LowLevel::allocateDataset(int size, OmDataWrapperPtr data)
{
	debug("hdf5verbose", "OmHDF5LowLevel: in %s...\n", __FUNCTION__);

	herr_t status;

	//Creates a new simple dataspace and opens it for access.
	int rank = 1;
	hsize_t dim = size / data->getSizeof();

	// TODO: fixme! use the max dim!
	hsize_t max = dim;
	hid_t dataspace_id = H5Screate_simple(rank, &dim, &max);
	if (dataspace_id < 0) {
		throw OmIoException("Could not create HDF5 dataspace.");
	}

	//Creates a dataset at the specified location.
	hid_t dataset_id = H5Dcreate2(fileId, getPath(), data->getHdf5FileType(),
                                     dataspace_id,
                                     H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	if (dataset_id < 0) {
		throw OmIoException("Could not create HDF5 dataset " + string(getPath()));
	}

	//if given data, then write it into new dataset
	if (NULL != data->getVoidPtr()) {
		status = H5Dwrite(dataset_id, data->getHdf5FileType(),
				  H5S_ALL, H5S_ALL, H5P_DEFAULT,
				  data->getVoidPtr());
		if (status < 0)
			throw OmIoException("Could not copy data into HDF5 dataset.");
	}

	//Closes the specified dataset.
	status = H5Dclose(dataset_id);
	if (status < 0) {
		throw OmIoException("Could not close HDF5 dataset.");
	}

	//Releases and terminates access to a dataspace.
	status = H5Sclose(dataspace_id);
	if (status < 0) {
		throw OmIoException("Could not close HDF5 dataspace.");
	}
}

OmDataWrapperPtr OmHdf5LowLevel::getNullDataWrapper(const hid_t dstype)
{
	return getDataWrapper(NULL, dstype, INVALID);
}

void OmHdf5LowLevel::printTypeInfo(const hid_t dstype)
{
	OmDataWrapperPtr p = getNullDataWrapper(dstype);
	printf("type is %s\n", p->getTypeAsString().c_str() );
}

int OmHdf5LowLevel::getSizeofType(const hid_t dstype)
{
	return getNullDataWrapper(dstype)->getSizeof();
}

OmDataWrapperPtr OmHdf5LowLevel::getDataWrapper(void* dataset,
						hid_t dstype,
						const OmDataAllocType allocType)
{
        switch( H5Tget_class( dstype ) ){
        case H5T_INTEGER:
                if( H5Tequal(dstype, H5T_NATIVE_UCHAR ) ){
                        return OmDataWrapper<uint8_t>::produce(dataset, allocType);
                } else if( H5Tequal(dstype, H5T_NATIVE_CHAR ) ){
                        return OmDataWrapper<int8_t>::produce(dataset, allocType);
                }else if( H5Tequal(dstype, H5T_NATIVE_UINT ) ){
                        return OmDataWrapper<uint32_t>::produce(dataset, allocType);
                }else if( H5Tequal(dstype, H5T_NATIVE_INT ) ){
                        return OmDataWrapper<int32_t>::produce(dataset, allocType);
                }else {
			throw OmIoException("unknown hdf5 integer type");
                }
                break;
        case H5T_FLOAT:
                return OmDataWrapper<float>::produce(dataset, allocType);
                break;
        default:
		throw OmIoException("unknown hdf5 type");
                break;
        }
}

void OmHdf5LowLevel::group_create(const char* path)
{
	debug("hdf5verbose", "OmHDF5LowLevel: in %s...\n", __FUNCTION__);

        //Creates a new empty group and links it into the file.
        hid_t group_id = H5Gcreate2(fileId, path, 0, H5P_DEFAULT, H5P_DEFAULT);
        if (group_id < 0) {
                throw OmIoException("Could not create HDF5 group.");
	}

        herr_t status = H5Gclose(group_id);
        if (status < 0) {
                throw OmIoException("Could not close HDF5 group.");
	}
}

/**
 *	Creates nested group tree.  Ignores already existing groups.
 */
void OmHdf5LowLevel::group_create_tree(const char* path)
{
	debug("hdf5verbose", "OmHDF5LowLevel: in %s...\n", __FUNCTION__);

	std::string curPath;
	foreach( const QString & folder, QString(path).split('/') ){
		curPath += folder.toStdString() + "/";

                //create if group does not exist
                if (!group_exists(curPath.c_str() ) ){
                        group_create(curPath.c_str() );
                }
        }
}

void OmHdf5LowLevel::group_delete(const char* path)
{
	debug("hdf5verbose", "OmHDF5LowLevel: in %s...\n", __FUNCTION__);

	//Closes the specified group.
	herr_t err = H5Gunlink(fileId, path);
	if (err < 0) {
		throw OmIoException("Could not unlink HDF5 group " + string(getPath()));
	}
}

/////////////////////////////////
///////          Dataset private
bool OmHdf5LowLevel::dataset_exists()
{
         debug("hdf5verbose", "OmHDF5LowLevel: in %s...\n", __FUNCTION__);

         hid_t dataset_id;

         //Try to open a data set
         //Turn off error printing idea from http://www.fiberbundle.net/index.html
         H5E_BEGIN_TRY {
                 dataset_id = H5Dopen2(fileId, getPath(), H5P_DEFAULT);
         } H5E_END_TRY

         //if failure, then assume doesn't exist
         if (dataset_id < 0)
                 return false;

         //Closes the specified dataset.
         herr_t ret = H5Dclose(dataset_id);
         if (ret < 0) {
                 throw OmIoException("Could not close HDF5 dataset " + string(getPath()));
         }

         return true;
}

void OmHdf5LowLevel::dataset_delete()
{
	debug("hdf5verbose", "OmHDF5LowLevel: in %s...\n", __FUNCTION__);

        //Removes the link to an object from a group.
        herr_t err = H5Gunlink(fileId, getPath());
        if (err < 0) {
                throw OmIoException("Could not unlink HDF5 dataset " + string(getPath()));
	}
}

void OmHdf5LowLevel::dataset_delete_create_tree()
{
	debug("hdf5verbose", "OmHDF5LowLevel: in %s...\n", __FUNCTION__);

        //get position of last slash
        string name_str(getPath());
        size_t pos_last_slash = name_str.find_last_of(string("/"));

        //if there was a slash
        if ((string::npos != pos_last_slash) && (pos_last_slash > 0)) {
                //split into group path and name
                string group_path(name_str, 0, pos_last_slash);

                group_create_tree(group_path.c_str());
        }
        //if data already exists, delete it
        if (dataset_exists()) {
                dataset_delete();
        }

}

Vector3i  OmHdf5LowLevel::dataset_image_get_dims()
{
	debug("hdf5verbose", "OmHDF5LowLevel: in %s...\n", __FUNCTION__);

	herr_t status;

	//Opens an existing dataset.
	hid_t dataset_id = H5Dopen2(fileId, getPath(), H5P_DEFAULT);
	if (dataset_id < 0) {
		throw OmIoException("Could not open HDF5 dataset " + string(getPath()));
	}

	//Returns an identifier for a copy of the dataspace for a dataset.
	hid_t dataspace_id = H5Dget_space(dataset_id);
	if (dataspace_id < 0) {
		throw OmIoException("Could not get HDF5 dataspace.");
	}

	//Determines the dimensionality of a dataspace.
	int rank = H5Sget_simple_extent_ndims(dataspace_id);
	if (rank < 0) {
		throw OmIoException("Could not determine rank of HDF5 dataspace.");
	}

	if (rank != 3){
		throw OmIoException("HDF5 dataspace not of rank 3.");
	}

	//Retrieves dataspace dimension size and maximum size.
	Vector3<hsize_t> dims;
	Vector3<hsize_t> maxdims;
	rank = H5Sget_simple_extent_dims(dataspace_id,
					 dims.array,
					 maxdims.array);
	if (rank < 0) {
		throw OmIoException("Could not determine dimensions of HDF5 dataspace.");
	}

	//Releases and terminates access to a dataspace.
	status = H5Sclose(dataspace_id);
	if (status < 0) {
		throw OmIoException("Could not close HDF5 dataspace.");
	}

	//Closes the specified dataset.
	status = H5Dclose(dataset_id);
	if (status < 0) {
		throw OmIoException("Could not close HDF5 dataset.");
	}

	debug("hdf5image", "dims are %d:%d:%d; maxdims are %d:%d:%d\n",
	      DEBUGV3(dims), DEBUGV3(maxdims));

	//flip from hdf5 version
	return Vector3i (dims.z, dims.y, dims.x);
}

void OmHdf5LowLevel::allocateChunkedDataset(const Vector3i& dataDims,
					    const Vector3i& chunkDims,
					    const OmVolDataType type)
{
	herr_t ret;
	int rank = 3;
	debug("hdf5verbose", "OmHDF5LowLevel: in %s...i\n", __FUNCTION__);

	//Creates a new property as an instance of a property list class.
	hid_t plist_id = H5Pcreate(H5P_DATASET_CREATE);
	if (plist_id < 0) {
		throw OmIoException("Could not create HDF5 property list.");
	}

	//Sets the size of the chunks used to store a chunked layout dataset.
	Vector3<hsize_t> flipped_chunk_dim(chunkDims.z, chunkDims.y, chunkDims.x);
	ret = H5Pset_chunk(plist_id, rank, flipped_chunk_dim.array);
	if (ret < 0) {
		throw OmIoException("Could not set HDF5 chunk size.");
	}
	hid_t whenAlloc = H5Pset_alloc_time( plist_id, H5D_ALLOC_TIME_EARLY);
	if (whenAlloc < 0) {
		throw OmIoException("Could not set HDF5 alloc time.");
	}

	//data dims
	Vector3<hsize_t> flipped_data_dims(dataDims.z, dataDims.y, dataDims.x);
	Vector3<hsize_t> flipped_max_data_dims;

	flipped_max_data_dims = Vector3<hsize_t>(dataDims.z, dataDims.y, dataDims.x);

	//Creates a new simple dataspace and opens it for access.
	hid_t dataspace_id = H5Screate_simple(rank,
					      flipped_data_dims.array,
					      flipped_max_data_dims.array);
	if (dataspace_id < 0) {
		throw OmIoException("Could not create HDF5 dataspace.");
	}

	//Creates a dataset at the specified location.
	hid_t type_id = OmVolumeTypeHelpers::getHDF5FileType(type);
	hid_t dataset_id = H5Dcreate2(fileId, getPath(), type_id, dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	if (dataset_id < 0) {
		throw OmIoException("Could not create HDF5 dataset " + string(getPath()));
	}

	//Terminates access to a property list.
	ret = H5Pclose(plist_id);
	if (ret < 0) {
		throw OmIoException("Could not close HDF5 property list.");
	}

	//Releases and terminates access to a dataspace.
	ret = H5Sclose(dataspace_id);
	if (ret < 0) {
		throw OmIoException("Could not close HDF5 dataspace.");
	}

	//Closes the specified dataset.
	ret = H5Dclose(dataset_id);
	if (ret < 0) {
		throw OmIoException("Could not close HDF5 dataset.");
	}
}

OmDataWrapperPtr OmHdf5LowLevel::readChunkVTK(DataBbox extent)
{
	debug("hdf5verbose", "OmHDF5LowLevel: in %s...\n", __FUNCTION__);

	//Opens an existing dataset.
	//hid_t H5Dopen(hid_t loc_id, const char *name  )
	hid_t dataset_id = H5Dopen2(fileId, getPath(), H5P_DEFAULT);
	if (dataset_id < 0) {
		throw OmIoException("Could not open HDF5 dataset " + string(getPath()));
	}

	hid_t dstype = H5Dget_type(dataset_id);

	//Returns an identifier for a copy of the dataspace for a dataset.
	//hid_t H5Dget_space(hid_t dataset_id  )
	hid_t dataspace_id = H5Dget_space(dataset_id);
	if (dataspace_id < 0) {
		throw OmIoException("Could not get HDF5 dataspace.");
	}

	//create start, stride, count, block
	//flip coordinates cuz thats how hdf5 likes it
	Vector3<hsize_t> start = extent.getMin();
	Vector3<hsize_t> start_flipped(start.z, start.y, start.x);

	Vector3<hsize_t> stride = Vector3i::ONE;
	Vector3<hsize_t> count = Vector3i::ONE;

	Vector3<hsize_t> block = extent.getUnitDimensions();
	Vector3<hsize_t> block_flipped(block.z, block.y, block.x);
	debug("hdf5image", "start:%i,%i,%i\n", DEBUGV3(start));
	debug("hdf5image", "block:%i,%i,%i\n", DEBUGV3(block));
	//Selects a hyperslab region to add to the current selected region.
	herr_t ret =
	    H5Sselect_hyperslab(dataspace_id,
				H5S_SELECT_SET,
				start_flipped.array,
				stride.array,
				count.array,
				block_flipped.array);
	if (ret < 0) {
		throw OmIoException("Could not select HDF5 hyperslab.");
	}

	//Creates a new simple dataspace and opens it for access.
	hid_t mem_dataspace_id = H5Screate_simple(3, block.array, NULL);
	if (mem_dataspace_id < 0) {
		throw OmIoException("Could not create scratch HDF5 dataspace to read data into.");
	}

	//setup image data
	Vector3i extent_dims = extent.getUnitDimensions();
	OmDataWrapperPtr imageData =
		OmImageDataIo::allocImageData(extent_dims,
					      getNullDataWrapper(dstype));

	ret = H5Dread(dataset_id, imageData->getHdf5MemoryType(), mem_dataspace_id, dataspace_id,
		      H5P_DEFAULT, imageData->getVTKptr()->GetScalarPointer());
	if (ret < 0) {
		throw OmIoException("Could not read HDF5 dataset " + string(getPath()));
	}

	H5Tclose( dstype );

	//Releases and terminates access to a dataspace.
	//herr_t H5Sclose(hid_t space_id  )
	ret = H5Sclose(mem_dataspace_id);
	if (ret < 0) {
		throw OmIoException("Could not close HDF5 scratch dataspace.");
	}

	ret = H5Sclose(dataspace_id);
	if (ret < 0) {
		throw OmIoException("Could not close HDF5 dataspace.");
	}

	//Closes the specified dataset.
	//herr_t H5Dclose(hid_t dataset_id  )
	ret = H5Dclose(dataset_id);
	if (ret < 0) {
		throw OmIoException("Could not close HDF5 dataset.");
	}

	return imageData;
}

void OmHdf5LowLevel::writeChunk(DataBbox extent,
				OmDataWrapperPtr data)
{
	debug("hdf5verbose", "OmHDF5LowLevel: in %s...\n", __FUNCTION__);

	//Opens an existing dataset.
	hid_t dataset_id = H5Dopen2(fileId, getPath(), H5P_DEFAULT);
	if (dataset_id < 0) {
		throw OmIoException("Could not open HDF5 dataset " + string(getPath()));
	}

	//Returns an identifier for a copy of the dataspace for a dataset.
	hid_t dataspace_id = H5Dget_space(dataset_id);
	if (dataspace_id < 0) {
		throw OmIoException("Could not get HDF5 dataspace.");
	}

	//create start, stride, count, block
	//flip coordinates cuz thats how hdf5 likes it
	Vector3<hsize_t> start = extent.getMin();
	Vector3<hsize_t> end   = extent.getMax();
	Vector3<hsize_t> start_flipped(start.z, start.y, start.x);

	Vector3<hsize_t> stride = Vector3i::ONE;
	Vector3<hsize_t> count = Vector3i::ONE;

	Vector3<hsize_t> block = extent.getUnitDimensions();
	Vector3<hsize_t> block_flipped(block.z, block.y, block.x);

	//Selects a hyperslab region to add to the current selected region.
	herr_t ret = H5Sselect_hyperslab(dataspace_id,
					 H5S_SELECT_SET,
					 start_flipped.array,
					 stride.array,
					 count.array,
					 block_flipped.array);
	if (ret < 0)
		throw OmIoException("Could not select HDF5 hyperslab.");

	//Creates a new simple dataspace and opens it for access.
	hid_t mem_dataspace_id = H5Screate_simple(3, block.array, block.array);
	if (mem_dataspace_id < 0)
		throw OmIoException("Could not create scratch HDF5 dataspace to read data into.");

	//setup image data
	Vector3i extent_dims = extent.getUnitDimensions();

	//Reads raw data from a dataset into a buffer.
	ret = H5Dwrite(dataset_id, data->getHdf5MemoryType(),
		       mem_dataspace_id, dataspace_id, H5P_DEFAULT,
		       data->getVoidPtr());
	if (ret < 0) {
		throw OmIoException("Could not read HDF5 dataset " + string(getPath()));
	}

	//Releases and terminates access to a dataspace.
	//herr_t H5Sclose(hid_t space_id  )
	ret = H5Sclose(mem_dataspace_id);
	if (ret < 0)
		throw OmIoException("Could not close HDF5 scratch dataspace.");

	ret = H5Sclose(dataspace_id);
	if (ret < 0)
		throw OmIoException("Could not close HDF5 dataspace.");

	//Closes the specified dataset.
	//herr_t H5Dclose(hid_t dataset_id  )
	ret = H5Dclose(dataset_id);
	if (ret < 0)
		throw OmIoException("Could not close HDF5 dataset.");
}

OmDataWrapperPtr OmHdf5LowLevel::readChunk(DataBbox extent)
{
	debug("hdf5verbose", "OmHDF5LowLevel: in %s...\n", __FUNCTION__);

	//Opens an existing dataset.
	//hid_t H5Dopen(hid_t loc_id, const char *name  )
	hid_t dataset_id = H5Dopen2(fileId, getPath(), H5P_DEFAULT);
	if (dataset_id < 0)
		throw OmIoException("Could not open HDF5 dataset " + string(getPath()) );

	hid_t dstype = H5Dget_type(dataset_id);
	//printTypeInfo( dstype );


	//Returns an identifier for a copy of the dataspace for a dataset.
	//hid_t H5Dget_space(hid_t dataset_id  )
	hid_t dataspace_id = H5Dget_space(dataset_id);
	if (dataspace_id < 0)
		throw OmIoException("Could not get HDF5 dataspace.");

	//create start, stride, count, block
	//flip coordinates cuz thats how hdf5 likes it
	Vector3<hsize_t> start = extent.getMin();
	Vector3<hsize_t> end   = extent.getMax();
	Vector3<hsize_t> start_flipped(start.z, start.y, start.x);

	//hsize_t* dims;
	//hsize_t* maxdims;
        //if(H5Sget_simple_extent_dims(dataspace_id, dims, maxdims) < 0)
        //	throw OmIoException("Could not get HDF5 data extent.");
        //if ((dims[0]<end.z)&&(dims[1]<end.y)&&(dims[2]<end.z))
        //	throw OmIoException("Tried to read data outside of Dataspace extent.");

	Vector3<hsize_t> stride = Vector3i::ONE;
	Vector3<hsize_t> count = Vector3i::ONE;

	Vector3<hsize_t> block = extent.getUnitDimensions();
	Vector3<hsize_t> block_flipped(block.z, block.y, block.x);
	debug("hdf5image", "start:%i,%i,%i\n", DEBUGV3(start));
	debug("hdf5image", "block:%i,%i,%i\n", DEBUGV3(block));
	//Selects a hyperslab region to add to the current selected region.
	herr_t ret =
	    H5Sselect_hyperslab(dataspace_id, H5S_SELECT_SET, start_flipped.array, stride.array, count.array,
				block_flipped.array);
	if (ret < 0)
		throw OmIoException("Could not select HDF5 hyperslab.");

	//Creates a new simple dataspace and opens it for access.
	hid_t mem_dataspace_id = H5Screate_simple(3, block.array, NULL);
	if (mem_dataspace_id < 0)
		throw OmIoException("Could not create scratch HDF5 dataspace to read data into.");

	//setup image data
	Vector3i extent_dims = extent.getUnitDimensions();

	void *imageData = malloc(extent_dims.x*extent_dims.y*extent_dims.z*(getSizeofType(dstype)));
	OmDataWrapperPtr data = getDataWrapper( imageData, dstype, MALLOC );

	H5Tclose( dstype );

	ret = H5Dread(dataset_id, data->getHdf5MemoryType(),
		      mem_dataspace_id, dataspace_id, H5P_DEFAULT,
		      imageData);
	if (ret < 0) {
		throw OmIoException("Could not read HDF5 dataset \""
				    + string(getPath()) + "\"");
	}

	//Releases and terminates access to a dataspace.
	//herr_t H5Sclose(hid_t space_id  )
	ret = H5Sclose(mem_dataspace_id);
	if (ret < 0)
		throw OmIoException("Could not close HDF5 scratch dataspace.");

	ret = H5Sclose(dataspace_id);
	if (ret < 0)
		throw OmIoException("Could not close HDF5 dataspace.");

	//Closes the specified dataset.
	//herr_t H5Dclose(hid_t dataset_id  )
	ret = H5Dclose(dataset_id);
	if (ret < 0)
		throw OmIoException("Could not close HDF5 dataset.");

	return data;
}

Vector3i OmHdf5LowLevel::dataset_get_dims()
{
	debug("hdf5verbose", "OmHDF5LowLevel: in %s...\n", __FUNCTION__);

	Vector3<hsize_t> dims(0,0,0);

	herr_t status;

	//Opens an existing dataset.
	hid_t dataset_id = H5Dopen2(fileId, getPath(), H5P_DEFAULT);
	if (dataset_id < 0) {
		throw OmIoException("Could not open HDF5 dataset " + string(getPath()));
	}

	//Returns an identifier for a copy of the dataspace for a dataset.
	hid_t dataspace_id = H5Dget_space(dataset_id);
	if (dataspace_id < 0)
		throw OmIoException("Could not get HDF5 dataspace.");

	//Determines the dimensionality of a dataspace.
	int rank = H5Sget_simple_extent_ndims(dataspace_id);
	if (rank < 0)
		throw OmIoException("Could not determine rank of HDF5 dataspace.");

	//Retrieves dataspace dimension size and maximum size.
	Vector3<hsize_t> maxdims;
	rank = H5Sget_simple_extent_dims(dataspace_id, dims.array, maxdims.array);
	if (rank < 0)
		throw OmIoException("Could not determine dimensions of HDF5 dataspace.");

	//Releases and terminates access to a dataspace.
	status = H5Sclose(dataspace_id);
	if (status < 0)
		throw OmIoException("Could not close HDF5 dataspace.");

	//Closes the specified dataset.
	status = H5Dclose(dataset_id);
	if (status < 0)
		throw OmIoException("Could not close HDF5 dataset.");

	//flip from hdf5 version
	return dims;
}
