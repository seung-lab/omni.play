#include "omHdf5LowLevel.h"
#include "omImageDataIo.h"
#include "common/omException.h"
#include "common/omVtk.h"
#include "common/omDebug.h"

#include <vtkImageData.h>
#include <QFile>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string_regex.hpp>
namespace bfa = boost::algorithm;

/////////////////////////////////
///////          File
void OmHdf5LowLevel::om_hdf5_file_create(string fpath)
{
	debug("hdf5", "OmHDF5LowLevel: in %s...\n", __FUNCTION__);

        QFile file(QString(fpath.c_str()));
        if(!file.exists()){
		H5E_BEGIN_TRY {
			hid_t fileId = H5Fcreate(fpath.c_str(), H5F_ACC_EXCL, H5P_DEFAULT, H5P_DEFAULT);
			om_hdf5_file_close_with_lock (fileId);
		} H5E_END_TRY
        }
}

hid_t OmHdf5LowLevel::om_hdf5_file_open_with_lock(string fpath)
{
	debug("hdf5", "OmHDF5LowLevel: in %s...\n", __FUNCTION__);

	hid_t fileId;
	fileId = H5Fopen(fpath.c_str(), H5F_ACC_RDWR, H5P_DEFAULT);
	
	if (fileId < 0) {
                throw OmIoException("Could not open HDF5 file.");
	}
        return fileId;
}

void OmHdf5LowLevel::om_hdf5_file_close_with_lock (hid_t fileId)
{
	debug("hdf5", "OmHDF5LowLevel: in %s...\n", __FUNCTION__);

        herr_t ret = H5Fclose(fileId);
        if (ret < 0)
                throw OmIoException("Could not close HDF5 file.");
}

/////////////////////////////////
///////          Group
bool OmHdf5LowLevel::om_hdf5_group_exists_with_lock(hid_t fileId, const char *name)
{
	debug("hdf5", "OmHDF5LowLevel: in %s...\n", __FUNCTION__);

	hid_t group_id;

	//Try to open a group
	//Turn off error printing idea from http://www.fiberbundle.net/index.html
	H5E_BEGIN_TRY {
		group_id = H5Gopen(fileId, name);
	} H5E_END_TRY
	    //if failure, then assume doesn't exist
	    if (group_id < 0)
		return false;

	//Closes the specified dataset. 
	herr_t status = H5Gclose(group_id);
	if (status < 0)
		throw OmIoException("Could not close HDF5 group.");

	return true;
}

void OmHdf5LowLevel::om_hdf5_dataset_image_create_tree_overwrite_with_lock(hid_t fileId, const char *name, Vector3 < int >dataDims,
					    Vector3 < int >chunkDims, int bytesPerSample, bool unlimited)
{
	debug("hdf5", "OmHDF5LowLevel: in %s...\n", __FUNCTION__);

	//create tree and delete old data if exists
	om_hdf5_dataset_delete_create_tree_with_lock(fileId, name);

	//create data
	om_hdf5_dataset_image_create_with_lock(fileId, name, dataDims, chunkDims, bytesPerSample, unlimited);
}

/**
 *	Trims the read to data within the extent of the dataset.  Buffers the rest with zeros.
 */
vtkImageData * OmHdf5LowLevel::om_hdf5_dataset_image_read_trim_with_lock(hid_t fileId, const char *name, DataBbox dataExtent, int bytesPerSample)
{
	debug("hdf5", "OmHDF5LowLevel: in %s...\n", __FUNCTION__);

	//get dims
	Vector3 < int >dims = om_hdf5_dataset_image_get_dims_with_lock(fileId, name);

	//create extent
	DataBbox dataset_extent = DataBbox(Vector3 < int >::ZERO, dims.x, dims.y, dims.z);

	//if data extent contains given extent, just read from data
	if (dataset_extent.contains(dataExtent)) {
		return om_hdf5_dataset_image_read_with_lock(fileId, name, dataExtent, bytesPerSample);
	}
	//intersect with given extent
	DataBbox intersect_extent = dataset_extent;
	intersect_extent.intersect(dataExtent);

	//if empty intersection, return blank data
	if (intersect_extent.isEmpty()) {
		return createBlankImageData(dataExtent.getUnitDimensions(), bytesPerSample);
	}
	//else merge intersection and read data
	//read intersection from source
	vtkImageData *intersect_image_data = om_hdf5_dataset_image_read_with_lock(fileId, name, intersect_extent, bytesPerSample);

	//create blanks data
	vtkImageData *filled_read_data = createBlankImageData(dataExtent.getUnitDimensions(), bytesPerSample);

	//copy intersected data to proper location in blank image data
	//normalize to min of intersection
	DataBbox intersect_norm_intersect_extent = intersect_extent;
	intersect_norm_intersect_extent.offset(-intersect_extent.getMin());

	//normalize to min of dataExtent
	DataBbox dataextent_norm_intersect_extent = intersect_extent;
	dataextent_norm_intersect_extent.offset(-dataExtent.getMin());

	//copy data
	copyImageData(filled_read_data, dataextent_norm_intersect_extent,	//copy to intersect region in dataExtent
		      intersect_image_data, intersect_norm_intersect_extent);	//from extent of intersection

	//delete read intersect data
	intersect_image_data->Delete();

	debug ("mesher1", "filled_read_data: %p, %i\n", filled_read_data, filled_read_data->GetReferenceCount());
	return filled_read_data;
}

void OmHdf5LowLevel::om_hdf5_dataset_image_write_trim_with_lock(hid_t fileId, const char *name, DataBbox dataExtent, int bytesPerSample,
				 vtkImageData * pImageData)
{
	debug("hdf5", "OmHDF5LowLevel: in %s...\n", __FUNCTION__);

	//get dims
	Vector3 < int >dims = om_hdf5_dataset_image_get_dims_with_lock(fileId, name);

	//create extent
	DataBbox dataset_extent = DataBbox(Vector3 < int >::ZERO, dims.x, dims.y, dims.z);

	//if data extent contains given extent, just write data
	if (dataset_extent.contains(dataExtent)) {
		om_hdf5_dataset_image_write_with_lock(fileId, name, dataExtent, bytesPerSample, pImageData);
		return;
	}
	//intersect with given extent
	DataBbox intersect_extent = dataset_extent;
	intersect_extent.intersect(dataExtent);

	//if empty intersection, just return
	if (intersect_extent.isEmpty())
		return;

	//else create alloc image data with just intersection
	vtkImageData *p_intersect_data = allocImageData(intersect_extent.getUnitDimensions(), bytesPerSample);

	//copy imagedata intersection into intersection data
	//normalize to min of intersection
	DataBbox intersect_norm_intersect_extent = intersect_extent;
	intersect_norm_intersect_extent.offset(-intersect_extent.getMin());

	//normalize to min of dataExtent
	DataBbox dataextent_norm_intersect_extent = intersect_extent;
	dataextent_norm_intersect_extent.offset(-dataExtent.getMin());

	//debug("FIXME", << intersect_norm_intersect_extent << endl;
	//debug("FIXME", << dataextent_norm_intersect_extent << endl;

	//copy data
	copyImageData(p_intersect_data, intersect_norm_intersect_extent,	//copy to extent of intersection
		      pImageData, dataextent_norm_intersect_extent);	//from intersection in dataExtent

	//write intersection
	om_hdf5_dataset_image_write_with_lock(fileId, name, intersect_extent, bytesPerSample, p_intersect_data);

	//delete temp intersect data
	p_intersect_data->Delete();
}


/////////////////////////////////
///////          Dataset Raw Data
void * OmHdf5LowLevel::om_hdf5_dataset_raw_read_with_lock(hid_t fileId, const char *name, int *size)
{
	debug("hdf5", "OmHDF5LowLevel: in %s...\n", __FUNCTION__);

	void *dataset_data;
	herr_t status;

	//Opens an existing dataset.
	hid_t dataset_id = H5Dopen(fileId, name);
	if (dataset_id < 0)
		throw OmIoException("Could not open HDF5 dataset.");

	printfDatasetCacheSize( dataset_id );

	//Returns an identifier for a copy of the dataspace for a dataset. 
	hid_t dataspace_id = H5Dget_space(dataset_id);
	if (dataspace_id < 0)
		throw OmIoException("Could not get HDF5 dataspace.");

	//Returns the amount of storage required for a dataset. 
	hsize_t dataset_size = H5Dget_storage_size(dataset_id);
	if (NULL != size)
		*size = (int)dataset_size;

	//Allocate memory to read into
	dataset_data = malloc(dataset_size);
	if (NULL == dataset_data)
		throw OmIoException("Could not allocate memory to read HDF5 dataset.");

	//Reads raw data from a dataset into a buffer. 
	status = H5Dread(dataset_id, H5T_NATIVE_UCHAR, H5S_ALL, H5S_ALL, H5P_DEFAULT, dataset_data);
	if (status < 0)
		throw OmIoException("Could not read HDF5 dataset.");

	//Releases and terminates access to a dataspace. 
	status = H5Sclose(dataspace_id);
	if (status < 0)
		throw OmIoException("Could not close HDF5 dataspace.");

	//Closes the specified dataset. 
	status = H5Dclose(dataset_id);
	if (status < 0)
		throw OmIoException("Could not close HDF5 dataset.");

	return dataset_data;
}

void OmHdf5LowLevel::om_hdf5_dataset_raw_create_tree_overwrite_with_lock(hid_t fileId, const char *name, int size, const void *data)
{
	debug("hdf5", "OmHDF5LowLevel: in %s...\n", __FUNCTION__);

	//create tree and delete old data if exists
	om_hdf5_dataset_delete_create_tree_with_lock(fileId, name);

	//create data
	om_hdf5_dataset_raw_create_with_lock(fileId, name, size, data);
}

/////////////////////////////////
/////////////////////////////////
///////// Private ///////////////
/////////////////////////////////
/////////////////////////////////
hid_t OmHdf5LowLevel::om_hdf5_bytesToHdf5Id(int bytes)
{
	debug("hdf5", "OmHDF5LowLevel: in %s...\n", __FUNCTION__);

	switch (bytes) {
	case 1:
		return H5T_NATIVE_UCHAR;
	case 4:
		return H5T_NATIVE_UINT;
	default:
		assert(false);
	}
}

/////////////////////////////////
///////          Group private
void OmHdf5LowLevel::om_hdf5_group_create_with_lock(hid_t fileId, const char *name)
{
	debug("hdf5", "OmHDF5LowLevel: in %s...\n", __FUNCTION__);

        //Creates a new empty group and links it into the file.
        hid_t group_id = H5Gcreate(fileId, name, 0);
        if (group_id < 0)
                throw OmIoException("Could not create HDF5 group.");

        herr_t status = H5Gclose(group_id);
        if (status < 0)
                throw OmIoException("Could not close HDF5 group.");
}

/**
 *	Creates nested group tree.  Ignores already existing groups.
 */
void OmHdf5LowLevel::om_hdf5_group_create_tree_with_lock(hid_t fileId, const char *name)
{
	debug("hdf5", "OmHDF5LowLevel: in %s...\n", __FUNCTION__);

        vector < string > name_split_vec;
        bfa::split(name_split_vec, name, bfa::is_any_of("/"));

        string name_rejoin_str;
        for (int i = 0; i < name_split_vec.size(); ++i) {
                //add split
                name_rejoin_str.append(name_split_vec[i]).append("/");
                //create if group does not exist
                if (!om_hdf5_group_exists_with_lock(fileId, name_rejoin_str.c_str())) {
                        om_hdf5_group_create_with_lock(fileId, name_rejoin_str.c_str());
                }
        }
}

void OmHdf5LowLevel::om_hdf5_group_delete_with_lock(hid_t fileId, const char *name)
{
	debug("hdf5", "OmHDF5LowLevel: in %s...\n", __FUNCTION__);

	//Closes the specified group. 
	herr_t err = H5Gunlink(fileId, name);
	if (err < 0)
		throw OmIoException("Could not unlink HDF5 group.");
}

/////////////////////////////////
///////          Dataset private
bool OmHdf5LowLevel::om_hdf5_dataset_exists_with_lock(hid_t fileId, const char *name)
{
	debug("hdf5", "OmHDF5LowLevel: in %s...\n", __FUNCTION__);

	hid_t dataset_id;

	//Try to open a data set
	//Turn off error printing idea from http://www.fiberbundle.net/index.html
	H5E_BEGIN_TRY {
		dataset_id = H5Dopen(fileId, name);
	} H5E_END_TRY
	
		  //if failure, then assume doesn't exist
		  if (dataset_id < 0)
			  return false;
	
	//Closes the specified dataset. 
	herr_t ret = H5Dclose(dataset_id);
	if (ret < 0)
		throw OmIoException("Could not close HDF5 dataset.");

	return true;
}

void OmHdf5LowLevel::om_hdf5_dataset_delete_with_lock(hid_t fileId, const char *name)
{
	debug("hdf5", "OmHDF5LowLevel: in %s...\n", __FUNCTION__);

        //Removes the link to an object from a group.
        herr_t err = H5Gunlink(fileId, name);
        if (err < 0)
                throw OmIoException("Could not unlink HDF5 dataset.");
}

void OmHdf5LowLevel::om_hdf5_dataset_delete_create_tree_with_lock(hid_t fileId, const char *name)
{
	debug("hdf5", "OmHDF5LowLevel: in %s...\n", __FUNCTION__);

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
void OmHdf5LowLevel::om_hdf5_dataset_raw_create_with_lock(hid_t fileId, const char *name, int size, const void *data)
{
	debug("hdf5", "OmHDF5LowLevel: in %s...\n", __FUNCTION__);

	herr_t status;

	//Creates a new simple dataspace and opens it for access. 
	int rank = 1;
	hsize_t dim = size;
	hsize_t max = dim ? dim : H5S_UNLIMITED;
	hid_t dataspace_id = H5Screate_simple(rank, &dim, NULL);
	if (dataspace_id < 0)
		throw OmIoException("Could not create HDF5 dataspace.");

	//Creates a dataset at the specified location. 
	hid_t dataset_id = H5Dcreate(fileId, name, H5T_NATIVE_UCHAR, dataspace_id, H5P_DEFAULT);
	if (dataset_id < 0)
		throw OmIoException("Could not create HDF5 dataset.");

	//if given data, then write it into new dataset
	if (NULL != data) {
		status = H5Dwrite(dataset_id, H5T_NATIVE_UCHAR, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);
		if (status < 0)
			throw OmIoException("Could copy data into HDF5 dataset.");
	}

	//Closes the specified dataset. 
	status = H5Dclose(dataset_id);
	if (status < 0)
		throw OmIoException("Could not close HDF5 dataset.");

	//Releases and terminates access to a dataspace. 
	status = H5Sclose(dataspace_id);
	if (status < 0)
		throw OmIoException("Could not close HDF5 dataspace.");
}

/////////////////////////////////
///////          ImageIo
Vector3 < int > OmHdf5LowLevel::om_hdf5_dataset_image_get_dims_with_lock(hid_t fileId, const char *name)
{
	debug("hdf5", "OmHDF5LowLevel: in %s...\n", __FUNCTION__);

	Vector3 < hsize_t > dims;

	herr_t status;

	//Opens an existing dataset.
	hid_t dataset_id = H5Dopen(fileId, name);
	if (dataset_id < 0)
		throw OmIoException("Could not open HDF5 dataset.");

	//Returns an identifier for a copy of the dataspace for a dataset. 
	hid_t dataspace_id = H5Dget_space(dataset_id);
	if (dataspace_id < 0)
		throw OmIoException("Could not get HDF5 dataspace.");

	//Determines the dimensionality of a dataspace. 
	int rank = H5Sget_simple_extent_ndims(dataspace_id);
	if (rank < 0)
		throw OmIoException("Could not determine rank of HDF5 dataspace.");
	if (rank != 3)
		throw OmIoException("HDF5 dataspace not of rank 3.");

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
	return Vector3 < int >(dims.z, dims.y, dims.x);
}

void OmHdf5LowLevel::om_hdf5_dataset_image_create_with_lock(hid_t fileId, const char *name, Vector3 < int >dataDims, Vector3 < int >chunkDims, int bytesPerSample, bool unlimited)
{
	debug("hdf5", "OmHDF5LowLevel: in %s...\n", __FUNCTION__);

	//Creates a new property as an instance of a property list class.
	hid_t plist_id = H5Pcreate(H5P_DATASET_CREATE);
	if (plist_id < 0)
		throw OmIoException("Could not create HDF5 property list.");

	//Sets the size of the chunks used to store a chunked layout dataset. 
	int rank = 3;
	Vector3 < hsize_t > flipped_chunk_dim(chunkDims.z, chunkDims.y, chunkDims.x);
	herr_t ret = H5Pset_chunk(plist_id, rank, flipped_chunk_dim.array);
	if (ret < 0)
		throw OmIoException("Could not set HDF5 chunk size.");

	//data dims
	Vector3 < hsize_t > flipped_data_dims(dataDims.z, dataDims.y, dataDims.x);
	Vector3 < hsize_t > flipped_max_data_dims =
	    unlimited ? Vector3 < hsize_t > (H5S_UNLIMITED, H5S_UNLIMITED,
					     H5S_UNLIMITED) : Vector3 < hsize_t > (dataDims.z, dataDims.y, dataDims.x);

	//Creates a new simple dataspace and opens it for access. 
	hid_t dataspace_id = H5Screate_simple(rank, flipped_data_dims.array, flipped_max_data_dims.array);
	if (dataspace_id < 0)
		throw OmIoException("Could not create HDF5 dataspace.");

	//Creates a dataset at the specified location. 
	hid_t type_id = om_hdf5_bytesToHdf5Id(bytesPerSample);
	hid_t dataset_id = H5Dcreate(fileId, name, type_id, dataspace_id, plist_id);
	if (dataset_id < 0)
		throw OmIoException("Could not create HDF5 dataset.");

	//Terminates access to a property list. 
	ret = H5Pclose(plist_id);
	if (ret < 0)
		throw OmIoException("Could not close HDF5 property list.");

	//Releases and terminates access to a dataspace. 
	ret = H5Sclose(dataspace_id);
	if (ret < 0)
		throw OmIoException("Could not close HDF5 dataspace.");

	//Closes the specified dataset. 
	ret = H5Dclose(dataset_id);
	if (ret < 0)
		throw OmIoException("Could not close HDF5 dataset.");
	//H5Fflush (fileId, H5F_SCOPE_GLOBAL);
}

vtkImageData * OmHdf5LowLevel::om_hdf5_dataset_image_read_with_lock(hid_t fileId, const char *name, DataBbox extent, int bytesPerSample)
{
	debug("hdf5", "OmHDF5LowLevel: in %s...\n", __FUNCTION__);

	vtkImageData *imageData;

	//Opens an existing dataset.
	//hid_t H5Dopen(hid_t loc_id, const char *name  ) 
	hid_t dataset_id = H5Dopen(fileId, name);
	if (dataset_id < 0)
		throw OmIoException("Could not open HDF5 dataset.");

	//Returns an identifier for a copy of the dataspace for a dataset. 
	//hid_t H5Dget_space(hid_t dataset_id  ) 
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
	imageData = allocImageData(extent_dims, bytesPerSample);

	//Reads raw data from a dataset into a buffer. 
	hid_t mem_type_id = om_hdf5_bytesToHdf5Id(bytesPerSample);
	ret =
	    H5Dread(dataset_id, mem_type_id, mem_dataspace_id, dataspace_id, H5P_DEFAULT,
		    imageData->GetScalarPointer());
	if (ret < 0)
		throw OmIoException("Could not read HDF5 dataset.");

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

	return imageData;
}


void OmHdf5LowLevel::om_hdf5_dataset_image_write_with_lock(hid_t fileId, const char *name, DataBbox extent, int bytesPerSample,  vtkImageData * imageData)
{
	debug("hdf5", "OmHDF5LowLevel: in %s...\n", __FUNCTION__);

	void *image_data_scalar_pointer;
	hid_t mem_type_id;

	//Opens an existing dataset.
	//hid_t H5Dopen(hid_t loc_id, const char *name  ) 
	hid_t dataset_id = H5Dopen(fileId, name);
	if (dataset_id < 0)
		throw OmIoException("Could not open HDF5 dataset.");

	//Returns an identifier for a copy of the datatype for a dataset. 
	//hid_t H5Dget_type(hid_t dataset_id  )
	hid_t dataset_type_id = H5Dget_type(dataset_id);

	//assert that dest datatype size matches desired size
	assert(H5Tget_size(dataset_type_id) == bytesPerSample);

	//Returns an identifier for a copy of the dataspace for a dataset. 
	//hid_t H5Dget_space(hid_t dataset_id  ) 
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
	//herr_t H5Sselect_hyperslab(hid_t space_id, H5S_seloper_t op, const hsize_t *start, const hsize_t *stride, const hsize_t *count, const hsize_t *block  ) 
	herr_t ret =
	    H5Sselect_hyperslab(dataspace_id, H5S_SELECT_SET, start_flipped.array, stride.array, count.array,
				block_flipped.array);
	if (ret < 0)
		throw OmIoException("Could not select HDF5 hyperslab.");

	//Creates a new simple dataspace and opens it for access. 
	//hid_t H5Screate_simple(int rank, const hsize_t * dims, const hsize_t * maxdims  ) 
	hid_t mem_dataspace_id = H5Screate_simple(3, block.array, block.array);
	if (mem_dataspace_id < 0)
		throw OmIoException("Could not create scratch HDF5 dataspace to read data into.");

	//setup image data
	Vector3 < int >extent_dims = extent.getUnitDimensions();
	int data_dims[3];
	imageData->GetDimensions(data_dims);
	assert(data_dims[0] == extent_dims.x && data_dims[1] == extent_dims.y && data_dims[2] == extent_dims.z);

	//assert(1 == imageData->GetNumberOfScalarComponents());
	if (1 != imageData->GetNumberOfScalarComponents())
		goto doclose;

	image_data_scalar_pointer = imageData->GetScalarPointer();

	//Reads raw data from a dataset into a buffer. 
	//herr_t H5Dread(hid_t dataset_id, hid_t mem_type_id, hid_t mem_space_id, hid_t file_space_id, hid_t xfer_plist_id, void * buf  )
	mem_type_id = om_hdf5_bytesToHdf5Id(imageData->GetScalarSize());
	ret = H5Dwrite(dataset_id, mem_type_id, mem_dataspace_id, dataspace_id, H5P_DEFAULT, image_data_scalar_pointer);
	if (ret < 0)
		throw OmIoException("Could not read HDF5 dataset.");

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
	size_t rdcc_nslots;
	size_t rdcc_nbytes;
	double rdcc_w0;

	herr_t err = H5Pget_chunk_cache( H5Dget_access_plist(dataset_id), &rdcc_nslots, &rdcc_nbytes, &rdcc_w0);

	debug("hdf5", "dataset cache info: Number of chunk slots in the raw data chunk cache: %s\n", qPrintable( QString::number(rdcc_nslots )));
	debug("hdf5", "dataset cache info: Total size of the raw data chunk cache, in bytes: %s\n", qPrintable( QString::number(rdcc_nbytes )));
	debug("hdf5", "dataset cache info: Preemption policy: %s\n",  qPrintable( QString::number(rdcc_w0)));	      
}

