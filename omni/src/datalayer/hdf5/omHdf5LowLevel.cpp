#include "common/omDebug.h"
#include "common/omException.h"
#include "common/omVtk.h"
#include "datalayer/hdf5/omHdf5LowLevel.h"
#include "datalayer/omDataPaths.h"
#include "utility/omImageDataIo.h"
#include "utility/omSystemInformation.h"

#include <vtkImageData.h>
#include <QFile>
#include <QStringList>

/////////////////////////////////
///////          File
void OmHdf5LowLevel::om_hdf5_file_create(string fpath)
{
	debug("hdf5verbose", "OmHDF5LowLevel: in %s...\n", __FUNCTION__);

        QFile file(QString::fromStdString(fpath));
        if(!file.exists()){
		hid_t fileId = H5Fcreate(fpath.c_str(), H5F_ACC_EXCL, H5P_DEFAULT, H5P_DEFAULT);
		if (fileId < 0) {
			const string errMsg = "Could not create HDF5 file: " + fpath + "\n";
			fprintf(stderr, "%s", errMsg.c_str());
			throw OmIoException(errMsg);
		}

		om_hdf5_file_close_with_lock(fileId);
        }
}

hid_t OmHdf5LowLevel::om_hdf5_file_open_with_lock(string fpath, const bool readOnly  )
{
	debug("hdf5", "%s: opened HDF file\n", __FUNCTION__ );
	debug("hdf5verbose", "OmHDF5LowLevel: in %s...\n", __FUNCTION__);

	const unsigned int totalCacheSizeMB = 256;

	// number of elements (objects) in the raw data chunk cache (default 521)
	//  should be a prime number (due to simplistic hashing algorithm)
	size_t rdcc_nelmts = 2011;

	size_t rdcc_nbytes = totalCacheSizeMB * 1024 * 1024; // total size of the raw data chunk cache (default 1MB)
	double rdcc_w0 = 0.75;  // preemption policy (default 0.75)
	int    mdc_nelmts  = 0;    // no longer used

	hid_t fapl = H5Pcreate(H5P_FILE_ACCESS); // defaults
	herr_t err = H5Pset_cache( fapl, mdc_nelmts, rdcc_nelmts, rdcc_nbytes, rdcc_w0 );
	if(err < 0) {
		throw OmIoException("Could not setup HDF5 file cache.");
	}

	hid_t fileId;
	if( readOnly ) {
		fileId = H5Fopen(fpath.c_str(), H5F_ACC_RDONLY, fapl);
	} else {
		fileId = H5Fopen(fpath.c_str(), H5F_ACC_RDWR, fapl);
	}

	if (fileId < 0) {
		const string errMsg = "Could not open HDF5 file: " + fpath + "\n";
                throw OmIoException(errMsg);
	}

	printfFileCacheSize( fileId );

        return fileId;
}

void OmHdf5LowLevel::om_hdf5_flush_with_lock(const hid_t fileId)
{
	H5Fflush(fileId, H5F_SCOPE_GLOBAL);
}

void OmHdf5LowLevel::om_hdf5_file_close_with_lock (hid_t fileId)
{
	debug("hdf5", "%s: closed HDF file\n", __FUNCTION__ );
	debug("hdf5verbose", "OmHDF5LowLevel: in %s...\n", __FUNCTION__);

	om_hdf5_flush_with_lock( fileId );
        herr_t ret = H5Fclose(fileId);
        if (ret < 0) {
                throw OmIoException("Could not close HDF5 file.");
	}
}

bool OmHdf5LowLevel::checkIfLinkExists(hid_t fileID, const char *name)
{
	return H5Lexists(fileID, name, H5P_DEFAULT);
}

/////////////////////////////////
///////          Group
bool OmHdf5LowLevel::om_hdf5_group_exists_with_lock(hid_t fileId, const char *name)
{
        H5E_BEGIN_TRY {
 		herr_t ret = H5Gget_objinfo(fileId, name, 0, NULL);
 		if( ret < 0 ){
 			return false;
 		}
 	} H5E_END_TRY

 	return true;
}

/**
 *	Trims the read to data within the extent of the dataset.  Buffers the rest with zeros.
 */
OmDataWrapperPtr OmHdf5LowLevel::om_hdf5_dataset_image_read_trim_with_lock(hid_t fileId, const char *name, DataBbox dataExtent)
{
        debug("hdf5verbose", "OmHDF5LowLevel: in %s...\n", __FUNCTION__);

        //printf("%s is the name\n", name);

        //get dims
        Vector3 < int >dims = om_hdf5_dataset_image_get_dims_with_lock(fileId, name);

        //create extent
        DataBbox dataset_extent = DataBbox(Vector3 < int >::ZERO, dims.x, dims.y, dims.z);

        //if data extent contains given extent, just read from data
        if (dataset_extent.contains(dataExtent)) {
		//assert(0 && "stop asking for stuff that doesn't exist");
                return om_hdf5_dataset_image_read_with_lock(fileId, name, dataExtent);
        }
        //intersect with given extent
        DataBbox intersect_extent = dataset_extent;
        intersect_extent.intersect(dataExtent);

        //if empty intersection, return blank data
        if (intersect_extent.isEmpty()) {
                //return OmImageDataIo::createBlankImageData(dataExtent.getUnitDimensions(), bytesPerSample);
        }
        //else merge intersection and read data
        //read intersection from source
        OmDataWrapperPtr intersect_image_data = om_hdf5_dataset_image_read_with_lock(fileId, name, intersect_extent);

        //create blanks data
        OmDataWrapperPtr filled_read_data = OmImageDataIo::createBlankImageData(dataExtent.getUnitDimensions(), intersect_image_data);

        //copy intersected data to proper location in blank image data
        //normalize to min of intersection
        DataBbox intersect_norm_intersect_extent = intersect_extent;
        intersect_norm_intersect_extent.offset(-intersect_extent.getMin());

        //normalize to min of dataExtent
        DataBbox dataextent_norm_intersect_extent = intersect_extent;
        dataextent_norm_intersect_extent.offset(-dataExtent.getMin());

        //copy data
        OmImageDataIo::copyImageData(filled_read_data, dataextent_norm_intersect_extent,        //copy to intersect region in dataExtent
                      intersect_image_data, intersect_norm_intersect_extent);   //from extent of intersection


        return filled_read_data;
}

void OmHdf5LowLevel::om_hdf5_dataset_image_write_trim_with_lock(hid_t fileId,
								const char *name,
								const DataBbox& dataExtent,
								OmDataWrapperPtr data)
{
        debug("hdf5verbose", "OmHDF5LowLevel: in %s...\n", __FUNCTION__);
        debug("hdf5verbose", "OmHDF5LowLevel: in %s: path is %s\n", __FUNCTION__, name);

        //get dims
        Vector3<int> dims = om_hdf5_dataset_image_get_dims_with_lock(fileId, name);
        debug("hdf5image", "dims: %i,%i,%i\n", DEBUGV3(dims));

        //create extent
        DataBbox dataset_extent = DataBbox(Vector3 < int >::ZERO, dims.x, dims.y, dims.z);

        //if data extent contains given extent, just write data
        if (dataset_extent.contains(dataExtent)) {
                om_hdf5_dataset_image_write_with_lock(fileId, name, dataExtent, data);
                return;
        }
        //intersect with given extent
        DataBbox intersect_extent = dataset_extent;
        intersect_extent.intersect(dataExtent);

        //if empty intersection, just return
        if (intersect_extent.isEmpty())
                return;

        //else create alloc image data with just intersection
        OmDataWrapperPtr p_intersect_data = OmImageDataIo::allocImageData(intersect_extent.getUnitDimensions(), data);

        //copy imagedata intersection into intersection data
        //normalize to min of intersection
        DataBbox intersect_norm_intersect_extent = intersect_extent;
        intersect_norm_intersect_extent.offset(-intersect_extent.getMin());

        //normalize to min of dataExtent
        DataBbox dataextent_norm_intersect_extent = intersect_extent;
        dataextent_norm_intersect_extent.offset(-dataExtent.getMin());

        //copy data
        OmImageDataIo::copyImageData(p_intersect_data, intersect_norm_intersect_extent, //copy to extent of intersection
                                      data, dataextent_norm_intersect_extent);    //from intersection in dataExtent

        //write intersection
        om_hdf5_dataset_image_write_with_lock(fileId, name, intersect_extent, p_intersect_data);
}

/////////////////////////////////
///////          Dataset Raw Data

/**
 * method used to read meshes and .dat files from disk
 */
OmDataWrapperPtr OmHdf5LowLevel::om_hdf5_dataset_raw_read_with_lock(hid_t fileId, const char *name, int *size)
{
	debug("hdf5verbose", "\nOmHDF5LowLevel: in %s...\n", __FUNCTION__);
	debug("hdf5verbose", "OmHDF5LowLevel: in %s: path is %s\n", __FUNCTION__, name);

	if(!om_hdf5_group_exists_with_lock(fileId, name)){
		*size = 0;
		return OmDataWrapperInvalid();
	}

	void *dataset_data;
	herr_t status;

	//Opens an existing dataset.
	hid_t dataset_id = H5Dopen2(fileId, name, H5P_DEFAULT);
	if (dataset_id < 0) {
		const string errMsg = "Could not open HDF5 dataset (even though it existed)" + string(name);
		throw OmIoException(errMsg);
	}

	//printfDatasetCacheSize( dataset_id );

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
		const string errMsg = "Could not open HDF5 dataset " + string(name);
		throw OmIoException(errMsg);
	}

	OmDataWrapperPtr data = getDataWrapper(dataset_data, dstype);
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

void OmHdf5LowLevel::printTypeInfo( hid_t dstype )
{
	switch( H5Tget_class( dstype ) ){
	case H5T_INTEGER:
		printf("type is int; ");
		if( H5Tequal(dstype, H5T_NATIVE_UCHAR ) ){
			printf("subtype is UCHAR; ");
		} else if( H5Tequal(dstype, H5T_NATIVE_UINT ) ){
			printf("subtype is UINT; ");
		}else {
			printf("subtype is unknown; ");
		}
		break;
	case H5T_FLOAT:
		printf("type is float");
		printf("subtype is unknown; ");
		break;
	default:
		printf("type is unknown");
		break;
	}

	printf("precision %lu\n", (long unsigned) H5Tget_precision(dstype));

}

OmDataWrapperPtr OmHdf5LowLevel::getDataWrapper(void * dataset, hid_t dstype )
{
        switch( H5Tget_class( dstype ) ){
        case H5T_INTEGER:
                if( H5Tequal(dstype, H5T_NATIVE_UCHAR ) ){
                        return OmDataWrapper<unsigned char>::produce(dataset);
                } else if( H5Tequal(dstype, H5T_NATIVE_CHAR ) ){
                        return OmDataWrapper<char>::produce(dataset);
                }else if( H5Tequal(dstype, H5T_NATIVE_UINT ) ){
                        return OmDataWrapper<unsigned int>::produce(dataset);
                }else if( H5Tequal(dstype, H5T_NATIVE_INT ) ){
                        return OmDataWrapper<int>::produce(dataset);
                }else {
			assert(0);
                }
                break;
        case H5T_FLOAT:
                return OmDataWrapper<float>::produce(dataset);
                break;
        default:
		assert(0 && "not a known type");
                break;
        }
}

OmDataWrapperPtr OmHdf5LowLevel::getNullDataWrapper(hid_t dstype )
{
        switch( H5Tget_class( dstype ) ){
        case H5T_INTEGER:
                if( H5Tequal(dstype, H5T_NATIVE_UCHAR ) ){
                        return OmDataWrapper<unsigned char>::produceNull();
                } else if( H5Tequal(dstype, H5T_NATIVE_CHAR ) ){
                        return OmDataWrapper<char>::produceNull();
                }else if( H5Tequal(dstype, H5T_NATIVE_UINT ) ){
                        return OmDataWrapper<unsigned int>::produceNull();
                }else if( H5Tequal(dstype, H5T_NATIVE_INT ) ){
                        return OmDataWrapper<int>::produceNull();
                }else {
			assert(0);
                }
                break;
        case H5T_FLOAT:
                return OmDataWrapper<float>::produceNull();
                break;
        default:
		assert(0 && "not a known type");
                break;
        }

	assert(0);
}

int OmHdf5LowLevel::getSizeofType(hid_t dstype)
{
        switch( H5Tget_class( dstype ) ){
        case H5T_INTEGER:
                if( H5Tequal(dstype, H5T_NATIVE_UCHAR ) ){
                        return sizeof(unsigned char);
                } else if( H5Tequal(dstype, H5T_NATIVE_UINT ) ){
                        return sizeof(unsigned int);
                } else if( H5Tequal(dstype, H5T_NATIVE_INT ) ){
                        return sizeof(int);
                } else {
                        assert(0 && "not a real int that we understand");
                        //return OmDataType::INVALID;
                }
                break;
        case H5T_FLOAT:
                return sizeof(float);
                break;
        default:
                assert(0 && "not a known type");
                break;
        }

	assert(0);
}


void OmHdf5LowLevel::om_hdf5_dataset_raw_create_tree_overwrite_with_lock(hid_t fileId, const char *name, int size, const OmDataWrapperPtr data)
{
	debug("hdf5verbose", "OmHDF5LowLevel: in %s...\n", __FUNCTION__);

	//create tree and delete old data if exists
	om_hdf5_dataset_delete_create_tree_with_lock(fileId, name);

	//create data
	om_hdf5_dataset_raw_create_with_lock(fileId, name, size, data);
}

/////////////////////////////////
///////          Group private
void OmHdf5LowLevel::om_hdf5_group_create_with_lock(hid_t fileId, const char *name)
{
	debug("hdf5verbose", "OmHDF5LowLevel: in %s...\n", __FUNCTION__);

        //Creates a new empty group and links it into the file.
        hid_t group_id = H5Gcreate2(fileId, name, 0, H5P_DEFAULT, H5P_DEFAULT);
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
void OmHdf5LowLevel::om_hdf5_group_create_tree_with_lock(hid_t fileId, const char *name)
{
	debug("hdf5verbose", "OmHDF5LowLevel: in %s...\n", __FUNCTION__);

	QString currentPath;
	foreach( QString folder, QString(name).split('/') ){
		currentPath += folder + "/";

		string curPathStr = currentPath.toStdString();

                //create if group does not exist
                if (!om_hdf5_group_exists_with_lock(fileId, curPathStr.c_str() ) ){
                        om_hdf5_group_create_with_lock(fileId, curPathStr.c_str() );
                }
        }
}

void OmHdf5LowLevel::om_hdf5_group_delete_with_lock(hid_t fileId, const char *name)
{
	debug("hdf5verbose", "OmHDF5LowLevel: in %s...\n", __FUNCTION__);

	//Closes the specified group.
	herr_t err = H5Gunlink(fileId, name);
	if (err < 0) {
		throw OmIoException("Could not unlink HDF5 group " + string(name));
	}
}

/////////////////////////////////
///////          Dataset private
bool OmHdf5LowLevel::om_hdf5_dataset_exists_with_lock(hid_t fileId, const char *name)
{
         debug("hdf5verbose", "OmHDF5LowLevel: in %s...\n", __FUNCTION__);

         hid_t dataset_id;

         //Try to open a data set
         //Turn off error printing idea from http://www.fiberbundle.net/index.html
         H5E_BEGIN_TRY {
                 dataset_id = H5Dopen2(fileId, name, H5P_DEFAULT);
         } H5E_END_TRY

         //if failure, then assume doesn't exist
         if (dataset_id < 0)
                 return false;

         //Closes the specified dataset.
         herr_t ret = H5Dclose(dataset_id);
         if (ret < 0) {
                 throw OmIoException("Could not close HDF5 dataset " + string(name));
         }

         return true;
}

void OmHdf5LowLevel::om_hdf5_dataset_delete_with_lock(hid_t fileId, const char *name)
{
	debug("hdf5verbose", "OmHDF5LowLevel: in %s...\n", __FUNCTION__);

        //Removes the link to an object from a group.
        herr_t err = H5Gunlink(fileId, name);
        if (err < 0) {
                throw OmIoException("Could not unlink HDF5 dataset " + string(name));
	}
}

void OmHdf5LowLevel::om_hdf5_dataset_delete_create_tree_with_lock(hid_t fileId, const char *name)
{
	debug("hdf5verbose", "OmHDF5LowLevel: in %s...\n", __FUNCTION__);

        //get position of last slash
        string name_str(name);
        size_t pos_last_slash = name_str.find_last_of(string("/"));

        //if there was a slash
        if ((string::npos != pos_last_slash) && (pos_last_slash > 0)) {
                //split into group path and name
                string group_path(name_str, 0, pos_last_slash);
                //string file_name(name_str, pos_last_slash + 1, name_str.size());
                //debug("FIXME", << "group:" << group_path << endl;
                //debug("FIXME", << "file:" <<  file_name << endl;

                //create group tree
                om_hdf5_group_create_tree_with_lock(fileId, group_path.c_str());
        }
        //if data already exists, delete it
        if (om_hdf5_dataset_exists_with_lock(fileId, name)) {
                om_hdf5_dataset_delete_with_lock(fileId, name);
        }

}

/////////////////////////////////
///////          Dataset Raw Data private
void OmHdf5LowLevel::om_hdf5_dataset_raw_create_with_lock(hid_t fileId,
							  const char *name,
							  int size,
							  OmDataWrapperPtr data)
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
	hid_t dataset_id = H5Dcreate2(fileId, name, data->getHdf5FileType(),
				      dataspace_id,
				      H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	if (dataset_id < 0) {
		throw OmIoException("Could not create HDF5 dataset " + string(name));
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

Vector3 < int > OmHdf5LowLevel::om_hdf5_dataset_image_get_dims_with_lock(hid_t fileId,
									 const char *name)
{
	debug("hdf5verbose", "OmHDF5LowLevel: in %s...\n", __FUNCTION__);

	Vector3 < hsize_t > dims;

	herr_t status;

	//Opens an existing dataset.
	hid_t dataset_id = H5Dopen2(fileId, name, H5P_DEFAULT);
	if (dataset_id < 0) {
		throw OmIoException("Could not open HDF5 dataset " + string(name));
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
	Vector3 < hsize_t > maxdims;
	rank = H5Sget_simple_extent_dims(dataspace_id, dims.array, maxdims.array);
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
	return Vector3 < int >(dims.z, dims.y, dims.x);
}

hid_t getHDF5type(const OmVolDataType type)
{
	switch(type.index()){
	case OmVolDataType::OM_INT8:
		return H5T_STD_I8LE;
	case OmVolDataType::OM_UINT8:
		return H5T_STD_U8LE;
	case OmVolDataType::OM_INT32:
		return H5T_STD_I32LE;
	case OmVolDataType::OM_UINT32:
		return H5T_STD_U32LE;
	case OmVolDataType::OM_FLOAT:
		return H5T_IEEE_F32LE;
	case OmVolDataType::UNKNOWN:
		assert(0 && "unknown data type--probably old file?");
		break;
	}

	assert(0 && "type not found");
}

void OmHdf5LowLevel::om_hdf5_dataset_image_create_with_lock(hid_t fileId,
							    const char *name,
							    const Vector3i& dataDims,
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
	Vector3 < hsize_t > flipped_data_dims(dataDims.z, dataDims.y, dataDims.x);
	Vector3 < hsize_t > flipped_max_data_dims;

	flipped_max_data_dims = Vector3<hsize_t>(dataDims.z, dataDims.y, dataDims.x);

	//Creates a new simple dataspace and opens it for access.
	hid_t dataspace_id = H5Screate_simple(rank,
					      flipped_data_dims.array,
					      flipped_max_data_dims.array);
	if (dataspace_id < 0) {
		throw OmIoException("Could not create HDF5 dataspace.");
	}

	//Creates a dataset at the specified location.
	hid_t type_id = getHDF5type(type);
	hid_t dataset_id = H5Dcreate2(fileId, name, type_id, dataspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
	if (dataset_id < 0) {
		throw OmIoException("Could not create HDF5 dataset " + string(name));
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

OmDataWrapperPtr OmHdf5LowLevel::om_hdf5_dataset_image_read_with_lock(hid_t fileId, const char *name, DataBbox extent)
{
	debug("hdf5verbose", "OmHDF5LowLevel: in %s...\n", __FUNCTION__);

	//Opens an existing dataset.
	//hid_t H5Dopen(hid_t loc_id, const char *name  )
	hid_t dataset_id = H5Dopen2(fileId, name, H5P_DEFAULT);
	if (dataset_id < 0) {
		throw OmIoException("Could not open HDF5 dataset " + string(name));
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
	Vector3 < hsize_t > start = extent.getMin();
	Vector3 < hsize_t > start_flipped(start.z, start.y, start.x);

	Vector3 < hsize_t > stride = Vector3i::ONE;
	Vector3 < hsize_t > count = Vector3i::ONE;

	Vector3 < hsize_t > block = extent.getUnitDimensions();
	Vector3 < hsize_t > block_flipped(block.z, block.y, block.x);
	debug("hdf5image", "start:%i,%i,%i\n", DEBUGV3(start));
	debug("hdf5image", "block:%i,%i,%i\n", DEBUGV3(block));
	//Selects a hyperslab region to add to the current selected region.
	herr_t ret =
	    H5Sselect_hyperslab(dataspace_id, H5S_SELECT_SET, start_flipped.array, stride.array, count.array,
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
	Vector3 < int >extent_dims = extent.getUnitDimensions();
	OmDataWrapperPtr imageData =
		OmImageDataIo::allocImageData(extent_dims,
					      getNullDataWrapper(dstype));

	ret = H5Dread(dataset_id, imageData->getHdf5MemoryType(), mem_dataspace_id, dataspace_id,
		      H5P_DEFAULT, imageData->getVTKPtr()->GetScalarPointer());
	if (ret < 0) {
		throw OmIoException("Could not read HDF5 dataset " + string(name));
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


void OmHdf5LowLevel::om_hdf5_dataset_image_write_with_lock(hid_t fileId,
							   const char *name,
							   const DataBbox& extent,
							   OmDataWrapperPtr data)
{
	debug("hdf5verbose", "OmHDF5LowLevel: in %s...\n", __FUNCTION__);

	//Opens an existing dataset.
	hid_t dataset_id = H5Dopen2(fileId, name, H5P_DEFAULT);
	if (dataset_id < 0) {
		throw OmIoException("Could not open HDF5 dataset " + string(name));
	}

	//Returns an identifier for a copy of the dataspace for a dataset.
	hid_t dataspace_id = H5Dget_space(dataset_id);
	if (dataspace_id < 0)
		throw OmIoException("Could not get HDF5 dataspace.");

	//create start, stride, count, block
	//flip coordinates cuz thats how hdf5 likes it
	Vector3 < hsize_t > start = extent.getMin();
	Vector3 < hsize_t > start_flipped(start.z, start.y, start.x);

	Vector3 < hsize_t > stride = Vector3i::ONE;
	Vector3 < hsize_t > count = Vector3i::ONE;

	Vector3 < hsize_t > block = extent.getUnitDimensions();
	Vector3 < hsize_t > block_flipped(block.z, block.y, block.x);

	//Selects a hyperslab region to add to the current selected region.
	herr_t ret =
	    H5Sselect_hyperslab(dataspace_id, H5S_SELECT_SET,
				start_flipped.array, stride.array, count.array,
				block_flipped.array);
	if (ret < 0)
		throw OmIoException("Could not select HDF5 hyperslab.");

	//Creates a new simple dataspace and opens it for access.
	hid_t mem_dataspace_id = H5Screate_simple(3, block.array, block.array);
	if (mem_dataspace_id < 0)
		throw OmIoException("Could not create scratch HDF5 dataspace to read data into.");

	//setup image data
	Vector3 < int >extent_dims = extent.getUnitDimensions();
	int data_dims[3];
	data->getVTKPtr()->GetDimensions(data_dims);
	assert(data_dims[0] == extent_dims.x);
	assert(data_dims[1] == extent_dims.y);
	assert(data_dims[2] == extent_dims.z);

	//assert(1 == imageData->GetNumberOfScalarComponents());
	if (1 != data->getVTKPtr()->GetNumberOfScalarComponents())
		goto doclose;

	//Reads raw data from a dataset into a buffer.
	ret = H5Dwrite(dataset_id, data->getHdf5MemoryType(),
		       mem_dataspace_id, dataspace_id, H5P_DEFAULT,
		       data->getVTKPtr()->GetScalarPointer());
	if (ret < 0) {
		throw OmIoException("Could not read HDF5 dataset " + string(name));
	}

 doclose:
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

void OmHdf5LowLevel::printfDatasetCacheSize( const hid_t dataset_id )
{
	if( !isDebugCategoryEnabled( "hdf5cache" ) ){
		return;
	}

	size_t rdcc_nslots;
	size_t rdcc_nbytes;
	double rdcc_w0;

	H5Pget_chunk_cache( H5Dget_access_plist(dataset_id), &rdcc_nslots, &rdcc_nbytes, &rdcc_w0);

	printf("dataset cache info: Number of chunk slots in the raw data chunk cache: %s\n", qPrintable( QString::number(rdcc_nslots )));
	printf("dataset cache info: Total size of the raw data chunk cache, in bytes: %s\n", qPrintable( QString::number(rdcc_nbytes )));
	printf("dataset cache info: Preemption policy: %s\n",  qPrintable( QString::number(rdcc_w0)));
}

void OmHdf5LowLevel::printfFileCacheSize( const hid_t fileId )
{
	if( !isDebugCategoryEnabled( "hdf5cache" ) ){
		return;
	}

	int    mdc_nelmts; // no longer used
	size_t rdcc_nelmts;
	size_t rdcc_nbytes;
	double rdcc_w0;

	H5Pget_cache(H5Fget_access_plist( fileId ), &mdc_nelmts, &rdcc_nelmts, &rdcc_nbytes, &rdcc_w0 );

	printf("file cache info: Number of elements in the raw data chunk cache: %s\n", qPrintable( QString::number( rdcc_nelmts )));
	printf("file cache info: Total size of the raw data chunk cache, in bytes: %s\n", qPrintable( QString::number(rdcc_nbytes )));
	printf("file cache info: Preemption policy: %s\n",  qPrintable( QString::number(rdcc_w0)));
}

void OmHdf5LowLevel::om_hdf5_dataset_write_raw_chunk_data(hid_t fileId, const char *name,
							  DataBbox extent, OmDataWrapperPtr data)
{
	debug("hdf5verbose", "OmHDF5LowLevel: in %s...\n", __FUNCTION__);

	//Opens an existing dataset.
	hid_t dataset_id = H5Dopen2(fileId, name, H5P_DEFAULT);
	if (dataset_id < 0) {
		throw OmIoException("Could not open HDF5 dataset " + string(name));
	}

	//Returns an identifier for a copy of the dataspace for a dataset.
	hid_t dataspace_id = H5Dget_space(dataset_id);
	if (dataspace_id < 0) {
		throw OmIoException("Could not get HDF5 dataspace.");
	}

	//create start, stride, count, block
	//flip coordinates cuz thats how hdf5 likes it
	Vector3 < hsize_t > start = extent.getMin();
	Vector3 < hsize_t > end   = extent.getMax();
	Vector3 < hsize_t > start_flipped(start.z, start.y, start.x);

	Vector3 < hsize_t > stride = Vector3i::ONE;
	Vector3 < hsize_t > count = Vector3i::ONE;

	Vector3 < hsize_t > block = extent.getUnitDimensions();
	Vector3 < hsize_t > block_flipped(block.z, block.y, block.x);

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
	Vector3 < int >extent_dims = extent.getUnitDimensions();

	//Reads raw data from a dataset into a buffer.
	ret = H5Dwrite(dataset_id, data->getHdf5MemoryType(),
		       mem_dataspace_id, dataspace_id, H5P_DEFAULT,
		       data->getVoidPtr());
	if (ret < 0) {
		throw OmIoException("Could not read HDF5 dataset " + string(name));
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

OmDataWrapperPtr OmHdf5LowLevel::om_hdf5_dataset_read_raw_chunk_data(const hid_t fileId, const char *name, DataBbox extent)
{
	debug("hdf5verbose", "OmHDF5LowLevel: in %s...\n", __FUNCTION__);


	//Opens an existing dataset.
	//hid_t H5Dopen(hid_t loc_id, const char *name  )
	hid_t dataset_id = H5Dopen2(fileId, name, H5P_DEFAULT);
	if (dataset_id < 0)
		throw OmIoException("Could not open HDF5 dataset " + string(name) );

	hid_t dstype = H5Dget_type(dataset_id);
	//printTypeInfo( dstype );


	//Returns an identifier for a copy of the dataspace for a dataset.
	//hid_t H5Dget_space(hid_t dataset_id  )
	hid_t dataspace_id = H5Dget_space(dataset_id);
	if (dataspace_id < 0)
		throw OmIoException("Could not get HDF5 dataspace.");

	//create start, stride, count, block
	//flip coordinates cuz thats how hdf5 likes it
	Vector3 < hsize_t > start = extent.getMin();
	Vector3 < hsize_t > end   = extent.getMax();
	Vector3 < hsize_t > start_flipped(start.z, start.y, start.x);

	//hsize_t* dims;
	//hsize_t* maxdims;
        //if(H5Sget_simple_extent_dims(dataspace_id, dims, maxdims) < 0)
        //	throw OmIoException("Could not get HDF5 data extent.");
        //if ((dims[0]<end.z)&&(dims[1]<end.y)&&(dims[2]<end.z))
        //	throw OmIoException("Tried to read data outside of Dataspace extent.");

	Vector3 < hsize_t > stride = Vector3i::ONE;
	Vector3 < hsize_t > count = Vector3i::ONE;

	Vector3 < hsize_t > block = extent.getUnitDimensions();
	Vector3 < hsize_t > block_flipped(block.z, block.y, block.x);
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
	Vector3 < int >extent_dims = extent.getUnitDimensions();

	void *imageData;
	imageData = malloc(extent_dims.x*extent_dims.y*extent_dims.z*(getSizeofType(dstype)));
	OmDataWrapperPtr data = getDataWrapper( imageData, dstype );

	H5Tclose( dstype );

	ret = H5Dread(dataset_id, data->getHdf5MemoryType(),
		      mem_dataspace_id, dataspace_id, H5P_DEFAULT,
		      imageData);
	if (ret < 0) {
		throw OmIoException("Could not read HDF5 dataset " + string(name));
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

Vector3< int > OmHdf5LowLevel::om_hdf5_dataset_get_dims_with_lock(hid_t fileId, const char *name)
{
	debug("hdf5verbose", "OmHDF5LowLevel: in %s...\n", __FUNCTION__);

	Vector3 < hsize_t > dims(0,0,0);

	herr_t status;

	//Opens an existing dataset.
	hid_t dataset_id = H5Dopen2(fileId, name, H5P_DEFAULT);
	if (dataset_id < 0) {
		throw OmIoException("Could not open HDF5 dataset " + string(name));
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
	Vector3 < hsize_t > maxdims;
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

bool OmHdf5LowLevel::isDatasetPathNameAChannel( const char *name )
{
	const string defaultName = OmDataPaths::getDefaultHDF5channelDatasetName();

	return (0 == defaultName.compare(name));
}
