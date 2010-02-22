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

#include <vmmlib/vmmlib.h>
using namespace vmml;

class vtkImageData;

class OmProjectData {
public:
	static void instantiateProjectData( QString fileNameAndPath );	
	static OmProjectData* Instance();
	static void Delete();
	
	static QString getFileNameAndPath();
	
	static void Create();
	static void Open();
	static void Close();
	static void Flush();
	
	static bool IsOpen() {return Instance()->mIsOpen;}

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

	bool mIsOpen;

	OmHdf5* hdfFile;
};

template< class T > 
void 
OmProjectData::ArchiveRead(const string &name, T* t) {
	assert(IsOpen());

	bool dataExists = Instance()->hdfFile->dataset_exists( name);
	assert( dataExists );
	
	//read dataset
	int size;
	char* p_data = (char*) Instance()->hdfFile->dataset_raw_read(name, &size);
	
	//create string stream to read from
	std::stringstream sstream;
	sstream.write( p_data, size );
	
	//read from stream
	OM_ARCHIVE_IN_CLASS ia(sstream);
	ia >> *t;
	
	//delete read data
	delete p_data;
}

template< class T > 
void 
OmProjectData::ArchiveWrite(const string &name, T* t) {
	assert(IsOpen());

	//create string stream to write to
	std::stringstream sstream;
	
	//archive data
	OM_ARCHIVE_OUT_CLASS oa(sstream);
	oa << *t;

	//generate string
	string str = sstream.str();
	
	//write dataset
	Instance()->hdfFile->dataset_raw_create_tree_overwrite( name, str.size(), str.c_str());

	printf("saved project file \"%s\", at path \"%s\"\n", qPrintable(getFileNameAndPath()), name.c_str() );
}

#endif
