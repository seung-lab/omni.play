#ifndef OM_PROJECT_DATA_H
#define OM_PROJECT_DATA_H

/*
 *	Wrapper for HDF5 data storage.
 *
 *	Brett Warne - bwarne@mit.edu - 3/30/09
 */

#include "volume/omVolumeTypes.h"
#include "common/omSerialization.h"
#include "utility/omDataLayer.h"
#include "utility/omDataReader.h"
#include "utility/omDataWriter.h"
#include "common/omStd.h"

#include <vmmlib/vmmlib.h>
using namespace vmml;

class vtkImageData;

class OmProjectData {
public:
	static void instantiateProjectData( QString fileNameAndPath, 
					    const bool autoOpenAndClose);
	static OmProjectData* Instance();
	static void Delete();
	
	static QString getFileNameAndPath();
	static QString getAbsoluteFileNameAndPath();

	static void Create();
	static void Open();
	static void Close();
	static void Flush();
	
	static bool IsOpen() {return Instance()->mIsOpen;}
	static bool IsReadOnly() {return Instance()->mIsReadOnly;}

	static OmDataLayer * GetDataLayer();
	static OmDataReader * GetDataReader();
	static OmDataWriter * GetDataWriter();

	//groups
	static bool GroupExists(OmHdf5Path path);
	static void GroupDelete(OmHdf5Path path);
	
	//data set
	static bool DataExists(OmHdf5Path path);
	
	//image data io
	static void CreateImageData(OmHdf5Path path, Vector3<int>* dataDims, Vector3<int>* chunkDims, int bytesPerSample);
	static vtkImageData* ReadImageData(OmHdf5Path path, const DataBbox &extent, int bytesPerSample);
	static void WriteImageData(OmHdf5Path path, DataBbox *extent, int bytesPerSample, vtkImageData *data);

	//raw data io
	static void* ReadRawData(OmHdf5Path path, int* size = NULL);
	static void WriteRawData(OmHdf5Path path, int size, const void* data);
	
	//archive io
	template< class T > static void ArchiveRead(const OmHdf5Path path, T* t);
	template< class T > static void ArchiveWrite(const OmHdf5Path path, T* t);
	
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

	bool mIsReadOnly;

	void setupDataLayer( QString fileNameAndPath, const bool autoOpenAndClose);
	OmDataLayer  * dataLayer;
	OmDataReader * dataReader;
	OmDataWriter * dataWriter;
};

template< class T > 
void 
OmProjectData::ArchiveRead( OmHdf5Path path, T* t) {
	assert(IsOpen());

	bool dataExists = Instance()->dataReader->dataset_exists( path );
	assert( dataExists );
	
	//read dataset
	int size;
	char* p_data = (char*) Instance()->dataReader->dataset_raw_read(path, &size);
	
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
OmProjectData::ArchiveWrite( OmHdf5Path path, T* t) {
	assert(IsOpen());

	if( Instance()->IsReadOnly() ){
		printf("not saving %s...\n", path.getString().c_str() );
		return;
	}

	//create string stream to write to
	std::stringstream sstream;
	
	//archive data
	OM_ARCHIVE_OUT_CLASS oa(sstream);
	oa << *t;

	//generate string
	string str = sstream.str();
	
	//write dataset
	Instance()->dataWriter->dataset_raw_create_tree_overwrite( path, str.size(), str.c_str());

	printf("saved project file \"%s\", at path \"%s\"\n", qPrintable(getFileNameAndPath()), path.getString().c_str() );
}

#endif
