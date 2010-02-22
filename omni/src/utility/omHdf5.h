#ifndef OM_HDF_H
#define OM_HDF_H

#include <QString>
#include <string>
using std::string;

#include "volume/omVolumeTypes.h"
#include "omHdf5helpers.h"

class vtkImageData;

class OmHdf5 
{
 public:
	OmHdf5( QString fileNameAndPath );

	string getFileNameAndPathCstr();
	QString getFileNameAndPath();

	void create();

	bool group_exists( string name );
	void group_delete( string name );

	bool dataset_exists( string name );

	void dataset_image_create_tree_overwrite( string & path, Vector3 < int >dataDims, 
						  Vector3 < int >chunkDims, int bytesPerSample);
	vtkImageData* dataset_image_read_trim( string name, DataBbox dataExtent, int bytesPerSample);

	void dataset_image_write_trim( string name, DataBbox dataExtent, 
					      int bytesPerSample, vtkImageData *pImageData);
	void* dataset_raw_read( string name, int* size = NULL);
	void dataset_raw_create_tree_overwrite( string name, int size, const void* data);

 private:
	QString m_fileNameAndPath;
};

#endif
