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

extern struct utsname uts;
#include <sys/utsname.h>

#include <vmmlib/vmmlib.h>
using namespace vmml;


#include <hdf5.h>

class vtkImageData;

//file
void om_hdf5_file_create(string fpath);
void om_hdf5_file_close(string fileName);


//group
hid_t om_hdf5_file_open_with_lock(string fpath);
void om_hdf5_file_close_with_lock (hid_t fileId);
void om_hdf5_group_create_with_lock(hid_t fileId, const char *name);
void om_hdf5_dataset_raw_create_with_lock(hid_t fileId, const char *name, int size, const void *data);
void om_hdf5_dataset_image_create_with_lock(hid_t fileId, const char *name, Vector3 < int >dataDims, Vector3 < int >chunkDims,
                             int bytesPerSample, bool unlimited);


bool om_hdf5_group_exists(string fileName, const char* name);
void om_hdf5_group_create(string fileName, const char* name);
void om_hdf5_group_delete(string fileName, const char* name);
void om_hdf5_group_create_tree(string fileName, const char* name);



//data set
bool om_hdf5_dataset_exists(string fileName, const char* name);
void om_hdf5_dataset_delete(string fileName, const char* name);
void om_hdf5_dataset_delete_create_tree(string fileName, const char* name);


//raw data
void om_hdf5_dataset_raw_create(string fileName, const char* name, int size, const void* data);
void om_hdf5_dataset_raw_create_tree_overwrite(string fileName, const char* name, int size, const void* data);
void* om_hdf5_dataset_raw_read(string fileName, const char* name, int* size = NULL);


//image data
Vector3<int> om_hdf5_dataset_image_get_dims(string fileName, const char* name);

void om_hdf5_dataset_image_create(string fileName, const char* name, Vector3<int> dataDims, Vector3<int> chunkDims, int bytesPerSample, bool unlimited = false);
void om_hdf5_dataset_image_create_tree_overwrite(string fileName, const char* name, Vector3<int> dataDims, Vector3<int> chunkDims, int bytesPerSample, bool unlimited = false);
void om_hdf5_dataset_raw_create_tree_overwrite_with_lock(hid_t fileId, const char *name, int size, const void *data);

vtkImageData* om_hdf5_dataset_image_read(string fileName, const char* name, DataBbox extent, int bytesPerSample);
vtkImageData* om_hdf5_dataset_image_read_trim(string fileName, const char* name, DataBbox dataExtent, int bytesPerSample);

void om_hdf5_dataset_image_write(string fileName, const char* name, DataBbox extent, int bytesPerSample, vtkImageData *data);
void om_hdf5_dataset_image_write_trim(string fileName, const char* name, DataBbox dataExtent, int bytesPerSample, vtkImageData *pImageData);


//utility
hid_t om_hdf5_bytesToHdf5Id(int bytes);

#endif
