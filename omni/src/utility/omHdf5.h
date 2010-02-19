#ifndef OM_MDF5_H
#define OM_MDF5_H

/*
 *
 *
 */

#include "volume/omVolumeTypes.h"
#include "common/omStd.h"
#include "common/omSerialization.h"
#include "system/omGarbage.h"


#include <vmmlib/vmmlib.h>
using namespace vmml;


#include <hdf5.h>

#define HDF5LOCK() 								\
	OmGarbage::Hdf5Lock();							\
	hid_t fileId;								\
	std::cerr << fileName;							\
	try { 									\
		fileId = om_hdf5_file_open_with_lock (fileName);		\
		std::cerr << "  with id of" << fileId << endl;
		

#define HDF5UNLOCK() 								\
	} catch (OmException e) { 						\
		assert (0);							\
		om_hdf5_file_close_with_lock (fileId);				\
		OmGarbage::Hdf5Unlock(); 					\
		throw (e);							\
	}									\
	om_hdf5_file_close_with_lock (fileId);					\
	OmGarbage::Hdf5Unlock();


class vtkImageData;

//file
void om_hdf5_file_create(const char* fpath);
hid_t om_hdf5_file_open(const char* fpath);
void om_hdf5_file_close(char * fileName);


//group
hid_t om_hdf5_file_open_with_lock(const char *fpath);
void om_hdf5_file_close_with_lock (hid_t fileId);
void om_hdf5_group_create_with_lock(hid_t fileId, const char *name);
void om_hdf5_dataset_raw_create_with_lock(hid_t fileId, const char *name, int size, const void *data);
void
om_hdf5_dataset_image_create_with_lock(hid_t fileId, const char *name, Vector3 < int >dataDims, Vector3 < int >chunkDims,
                             int bytesPerSample, bool unlimited);


bool om_hdf5_group_exists(char * fileName, const char* name);
void om_hdf5_group_create(char * fileName, const char* name);
void om_hdf5_group_delete(char * fileName, const char* name);
void om_hdf5_group_create_tree(char * fileName, const char* name);



//data set
bool om_hdf5_dataset_exists(char * fileName, const char* name);
void om_hdf5_dataset_delete(char * fileName, const char* name);
void om_hdf5_dataset_delete_create_tree(char * fileName, const char* name);


//raw data
void om_hdf5_dataset_raw_create(char * fileName, const char* name, int size, const void* data);
void om_hdf5_dataset_raw_create_tree_overwrite(char * fileName, const char* name, int size, const void* data);
void* om_hdf5_dataset_raw_read(char * fileName, const char* name, int* size = NULL);


//image data
Vector3<int> om_hdf5_dataset_image_get_dims(char * fileName, const char* name);

void om_hdf5_dataset_image_create(char * fileName, const char* name, Vector3<int> dataDims, Vector3<int> chunkDims, int bytesPerSample, bool unlimited = false);
void om_hdf5_dataset_image_create_tree_overwrite(char * fileName, const char* name, Vector3<int> dataDims, Vector3<int> chunkDims, int bytesPerSample, bool unlimited = false);

vtkImageData* om_hdf5_dataset_image_read(char * fileName, const char* name, DataBbox extent, int bytesPerSample);
vtkImageData* om_hdf5_dataset_image_read_trim(char * fileName, const char* name, DataBbox dataExtent, int bytesPerSample);

void om_hdf5_dataset_image_write(char * fileName, const char* name, DataBbox extent, int bytesPerSample, vtkImageData *data);
void om_hdf5_dataset_image_write_trim(char * fileName, const char* name, DataBbox dataExtent, int bytesPerSample, vtkImageData *pImageData);


//utility
hid_t om_hdf5_bytesToHdf5Id(int bytes);




//serialization

/*
 *	Templated archive read/in method.
 */

template< class T >
inline
void
om_hdf5_archive_read(char * fileName, const char* name, T* t) {
	//assert data exists
	assert(om_hdf5_dataset_exists(fileName, name));
	
	//read dataset
	int size;
	char* p_data = (char*) om_hdf5_dataset_raw_read(fileName, name, &size);
	
	//create string stream to read from
	std::stringstream sstream;
	sstream.write( p_data, size );
	
	//read from stream
	OM_ARCHIVE_IN_CLASS ia(sstream);
	ia >> *t;
	
	//delete read data
	delete p_data;
}


/*
 *	Templated archive write/out method.
 */
template< class T >
inline
void
om_hdf5_archive_write(char * fileName, const char* name, T* t) {

	//create string stream to write to
	std::stringstream sstream;
	
	//archive data
	OM_ARCHIVE_OUT_CLASS oa(sstream);
	oa << *t;

	//generate string
	string str = sstream.str();
	
	//write dataset
	om_hdf5_dataset_raw_create_tree_overwrite(fileName, name, str.size(), str.c_str());
}



#endif
