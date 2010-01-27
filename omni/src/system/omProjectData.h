#ifndef OM_PROJECT_DATA_H
#define OM_PROJECT_DATA_H

/*
 *	Wrapper for HDF5 data storage.
 *
 *	Brett Warne - bwarne@mit.edu - 3/30/09
 */



#include "volume/omVolumeTypes.h"
#include "utility/omHdf5.h"
#include "common/omStd.h"


#include <hdf5.h>


class vtkImageData;


class OmProjectData {

public:
	
	static OmProjectData* Instance();
	static void Delete();
	
	static string GetFilePath();
	
	static void Create();
	static void Open();
	static void Close();
	static void Flush();
	
	static bool IsOpen();

	
	
	//groups
	static bool GroupExists(string &path);
	static void GroupDelete(string &path);
	
	//data set
	static bool DataExists(string &path);
	
	//image data io
	static void CreateImageData(string &path, Vector3<int> dataDims, Vector3<int> chunkDims, int bytesPerSample);
	static vtkImageData* ReadImageData(string &path, const DataBbox &extent, int bytesPerSample);
	static void WriteImageData(string &path, const DataBbox &extent, int bytesPerSample, vtkImageData *data);
	
	//raw data io
	static void* ReadRawData(string &path, int* size = NULL);
	static void WriteRawData(string &path, int size, const void* data);
	
	//archive io
	template< class T > static void ArchiveRead(const string &path, T* t);
	template< class T > static void ArchiveWrite(const string &path, T* t);
	
	
protected:
	// singleton constructor, copy constructor, assignment operator protected
	OmProjectData();
	~OmProjectData();
	OmProjectData(const OmProjectData&);
	OmProjectData& operator= (const OmProjectData&);

	
private:
	//singleton
	static OmProjectData* mspInstance;
	
	hid_t mFileId;
};



template< class T > 
void 
OmProjectData::ArchiveRead(const string &path, T* t) {
	assert(IsOpen());
	om_hdf5_archive_read<T>(Instance()->mFileId, path.c_str(), t);
}


template< class T > 
void 
OmProjectData::ArchiveWrite(const string &path, T* t) {
	assert(IsOpen());
	om_hdf5_archive_write<T>(Instance()->mFileId, path.c_str(), t);
}



#endif
