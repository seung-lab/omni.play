
#include "common/omStd.h"

#include "volume/omDataVolume.h"
#include "volume/omSegmentation.h"
#include "volume/omVolumeTypes.h"
#include "utility/omImageDataIo.h"
#include "system/omPreferences.h"
#include "system/omPreferenceDefinitions.h"
#include "system/omProjectData.h"
#include "system/omProject.h"
#include "utility/omHdf5.h"

#include <vmmlib/vmmlib.h>
using namespace vmml;

#include <vtkImageData.h>

#include <hdf5.h>
#include "system/omDebug.h"



void sample_write();


int main(int argc, char* argv[])
{


	
	switch(argc) {
		case 3:
			cout << argv[1] << " " << argv[2] << endl;
			break;

		default:
			cout << "Usage:" << endl;
			cout << argv[0] << " [directory] [pattern]" << endl;
			return 0;
	}
	
	
	string dpath(argv[1]);
	string regex(argv[2]);
	
	/*
	std::list<string> matches;
	om_imagedata_regex_match_dir_contents(dpath, regex, matches);
	cout << matches.size() << endl;
	
	//get dims
	Vector3<int> dims = om_imagedata_get_dims(dpath, matches);
	cout << dims << endl;
	
	OmDataVolume data_volume;
	data_volume.SetDataDimensions(dims);
	data_volume.SetDirectoryPath(dpath);
	data_volume.SetFileNames(matches);
	*/
	
	//DataBbox extent = DataBbox(Vector3i(0,0,0), Vector3i(50,50,10));
	//vtkImageData *data = data_volume.Read(extent, 4);	
	//printImageData(data);
	
	
	char filename[] = "project.omni";
	OmProject::New("./", filename);
	
	OmMipVolume mip_volume;
	mip_volume.SetSourceDirectoryPath(dpath);
	mip_volume.SetSourceFilenameRegex(regex);
	mip_volume.Build();
	
	OmProject::Save();
	
	//om_imagedata_create_symlink_dir("symlinks/", argv[1], matches);
	
	
	
	/*
	const char* filepath = "test.h5";
	const char* datapath = "main";
	
	om_hdf5_file_create(filepath);
	hid_t file_id = om_hdf5_file_open(filepath);
	om_hdf5_dataset_raw_create_tree_overwrite(file_id, datapath, 0, NULL);
	om_hdf5_file_close(file_id);
	 */
}





void sample_write() {
	
	herr_t ret;
	const char* filepath = "test.h5";
	const char* datapath = "main";
	hid_t hdf5_type = H5T_NATIVE_UINT;
	
	//Creates HDF5 files. 
	//hid_t H5Fcreate(const char *name, unsigned flags, hid_t create_id, hid_t access_id  ) 
	hid_t file_id = H5Fcreate(filepath, H5F_ACC_EXCL, H5P_DEFAULT, H5P_DEFAULT);
	
	//Creates a new property as an instance of a property list class.
	//hid_t H5Pcreate(hid_t cls_id  ) 
	hid_t plist_id = H5Pcreate(H5P_DATASET_CREATE);
	
	//Sets the size of the chunks used to store a chunked layout dataset. 
	//herr_t H5Pset_chunk(hid_t plist, int ndims, const hsize_t * dim  ) 
	int rank = 3;
	hsize_t dim[] = {128, 128, 128};
	ret = H5Pset_chunk(plist_id, rank, dim);
	assert(ret >= 0);
	
	//Creates a new simple dataspace and opens it for access. 
	//hid_t H5Screate_simple(int rank, const hsize_t * dims, const hsize_t * maxdims  ) 
	hsize_t dims[] = {256, 256, 256};
	hid_t dataspace_id = H5Screate_simple(rank, dims, dims);
	
	//Creates a dataset at the specified location. 
	//hid_t H5Dcreate(hid_t loc_id, const char *name, hid_t type_id, hid_t space_id, hid_t create_plist_id  ) 
	hid_t dataset_id = H5Dcreate(file_id, datapath, hdf5_type, dataspace_id, plist_id);
	
	//Writes raw data from a buffer to a dataset. 
	//herr_t H5Dwrite(hid_t dataset_id, hid_t mem_type_id, hid_t mem_space_id, hid_t file_space_id, hid_t xfer_plist_id, const void * buf  ) 
	void* zeros_buf = malloc( dims[0] * dims[1] * dims[2] * sizeof(unsigned int) );
	ret = H5Dwrite(dataset_id, hdf5_type, H5S_ALL, H5S_ALL, H5P_DEFAULT, zeros_buf);
	
	//Terminates access to a property list. 
	ret = H5Pclose(plist_id);
	assert(ret >= 0);
	
	//Closes the specified dataset.
	ret = H5Dclose(dataset_id);
	assert(ret >= 0);
	
	//Terminates access to an HDF5 file. 
	ret = H5Fclose(file_id);
	assert(ret >= 0);
}

