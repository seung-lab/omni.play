#include "omHdf5.h"
#include <stdlib.h>

OmHdf5::OmHdf5( QString fileNameAndPath )
{
	m_fileNameAndPath = fileNameAndPath;
}

QString OmHdf5::getFileNameAndPath()
{
	return m_fileNameAndPath;
}

string OmHdf5::getFileNameAndPathCstr()
{
	return m_fileNameAndPath.toStdString();
}

void OmHdf5::create()
{
	om_hdf5_file_create( getFileNameAndPathCstr() );
}

bool OmHdf5::group_exists( string name )
{
	return om_hdf5_group_exists(getFileNameAndPathCstr(), name.c_str());
}

void OmHdf5::group_delete( string name )
{
	om_hdf5_group_delete(getFileNameAndPathCstr(), name.c_str());
}

bool OmHdf5::dataset_exists( string name )
{
	return om_hdf5_dataset_exists(getFileNameAndPathCstr(), name.c_str());
}

void OmHdf5::dataset_image_create_tree_overwrite( string & name, Vector3 < int >dataDims, 
						  Vector3 < int >chunkDims, int bytesPerSample) 
{
	om_hdf5_dataset_image_create_tree_overwrite(getFileNameAndPathCstr(), name.c_str(), 
						    dataDims, chunkDims, bytesPerSample );
}

vtkImageData* OmHdf5::dataset_image_read_trim( string name, DataBbox dataExtent, int bytesPerSample)
{
	return om_hdf5_dataset_image_read_trim(getFileNameAndPathCstr(), name.c_str(), dataExtent, bytesPerSample);
}

void OmHdf5::dataset_image_write_trim( string name, DataBbox dataExtent, 
				       int bytesPerSample, vtkImageData *pImageData)
{
	om_hdf5_dataset_image_write_trim(getFileNameAndPathCstr(), name.c_str(), 
					 dataExtent, bytesPerSample, pImageData);
}

void* OmHdf5::dataset_raw_read( string name, int* size)
{
	return om_hdf5_dataset_raw_read(getFileNameAndPathCstr(), name.c_str(), size);
}

void OmHdf5::flush ()
{
	int magicNumberOmega = (1024 * 1024 * 8);
	do {
		OmGarbage::Hdf5Lock ();
		OmGarbage::Lock ();
		hid_t fileId = om_hdf5_file_open_with_lock (getFileNameAndPathCstr());
		int sizeOut = 0;
		while (mQueue.size()) {
			OmHdf5DataSet * dataSet = mQueue.dequeue();
			om_hdf5_dataset_raw_create_tree_overwrite_with_lock(fileId, dataSet->name.c_str(), dataSet->size, dataSet->data);
			debug ("hdf5bulk", "Qequeued and wrote to %s, size %i\n", dataSet->name.c_str(), dataSet->size);
			free ((void*)dataSet->data);
			sizeOut += dataSet->size;
			if (sizeOut > magicNumberOmega) {
				debug ("hdf5bulk", "Omega!\n");
				break;
			}
		}
		om_hdf5_file_close_with_lock(fileId);
		OmGarbage::Unlock();
		OmGarbage::Hdf5Unlock();
	} while (mQueue.size());
}

void OmHdf5::dataset_raw_create_tree_overwrite( string name, int size, const void* data, bool bulk)
{
	if (!size) return;
	if (bulk) {
		OmGarbage::Lock ();
		void * copy = malloc (size);
		memcpy ((void*)data, copy, size);
		mQueue.enqueue (new OmHdf5DataSet (name, size, copy));
		OmGarbage::Unlock ();
	} else {
		om_hdf5_dataset_raw_create_tree_overwrite(getFileNameAndPathCstr(), name.c_str(), size, data);
	}
}

