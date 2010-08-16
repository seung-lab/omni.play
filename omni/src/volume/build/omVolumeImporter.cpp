#include "system/cache/omMipVolumeCache.h"
#include "volume/omLoadImageThread.h"
#include "datalayer/omDataReader.h"
#include "datalayer/omDataWriter.h"
#include "project/omProject.h"
#include "utility/omImageDataIo.h"
#include "utility/omTimer.h"
#include "volume/build/omVolumeImporter.hpp"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataPaths.h"
#include "datalayer/omDataWrapper.h"
#include "system/omProjectData.h"

#include <QImage>
#include <QThreadPool>
#include <QMutexLocker>

template <typename T>
OmVolumeImporter<T>::OmVolumeImporter(T* vol)
	: vol(vol)
{
}

// copy data to MIP 0
template <typename T>
bool OmVolumeImporter<T>::import(OmDataPath & dataset)
{
	if(areImportFilesImages()){
		return importImageStack();
	}

	return importHDF5(dataset);
}

template <typename T>
bool OmVolumeImporter<T>::areImportFilesImages()
{
	return vol->areImportFilesImages();
}

template <typename T>
bool OmVolumeImporter<T>::importHDF5(OmDataPath & dataset)
{
	//dim of leaf coords
	const Vector3i leaf_mip_dims = vol->MipLevelDimensionsInMipChunks(0);

	OmDataPath leaf_volume_path;
	leaf_volume_path.setPathQstr( vol->MipLevelInternalDataPath(0) );

	printf("\timporting data...\n");
	fflush(stdout);

	OmTimer import_timer;
	import_timer.start();

	bool initialized = false;

	//for all coords
	for (int z = 0; z < leaf_mip_dims.z; ++z) {
		for (int y = 0; y < leaf_mip_dims.y; ++y) {
			for (int x = 0; x < leaf_mip_dims.x; ++x) {

				//get chunk data bbox
				const OmMipChunkCoord chunk_coord = OmMipChunkCoord(0, x, y, z);
				DataBbox chunk_data_bbox = vol->MipCoordToDataBbox(chunk_coord, 0);

				//read chunk image data from source
				OmDataWrapperPtr p_img_data =
					OmImageDataIo::om_imagedata_read_hdf5(vol->mSourceFilenamesAndPaths,
									      chunk_data_bbox,
									      dataset);
				if(!initialized) {
					initialized = true;
					vol->AllocInternalData(p_img_data);
				}

				//write to project data
				OmProjectData::GetDataWriter()->dataset_image_write_trim(leaf_volume_path,
											 &chunk_data_bbox,
											 p_img_data);
			}
		}
	}

	import_timer.stop();

	printf("done in %.6f secs\n",import_timer.s_elapsed());
	return true;
}

template <typename T>
void OmVolumeImporter<T>::figureOutNumberOfBytesImg()
{
	const int depth = QImage(vol->mSourceFilenamesAndPaths[0].absoluteFilePath()).depth();

	int numberOfBytes = 1;
	if(32 == depth){
		numberOfBytes=4;
	}

	vol->SetBytesPerSample(numberOfBytes);
}

template <typename T>
std::pair<int,QString> OmVolumeImporter<T>::getNextImgToProcess()
{
	QMutexLocker lock(&mutex);

	if(vol->mSourceFilenamesAndPaths.size() == mSliceNum){
		return std::pair<int,QString>(-1,"");
	}

	const QString ret = vol->mSourceFilenamesAndPaths[mSliceNum].absoluteFilePath();

	return std::pair<int,QString>(mSliceNum++, ret);
}

template <typename T>
bool OmVolumeImporter<T>::importImageStack()
{
	printf("\timporting data...\n");
	fflush(stdout);

	//timer start
	OmTimer import_timer;
	import_timer.start();

	figureOutNumberOfBytesImg();

	// alloc must happen after setBytesPerSample....
	if( 1 == vol->GetBytesPerSample() ){
		vol->ucharData->AllocMemMapFiles();
	} else {
		assert(0 && "don't know if float or uint32_t");
	}

	mSliceNum = 0;

	const int maxThreads=1;
	QThreadPool threads;
	threads.setMaxThreadCount(maxThreads);
	for(int i=0; i<maxThreads; ++i){
		OmLoadImageThread<T>* t = new OmLoadImageThread<T>(this, vol);
		threads.start(t);
	}
	threads.waitForDone();

	printf("\ndone with image import; copying to HDF5 file...\n");

	// silly way to allocate internal data
	foreach(const OmMipChunkCoord & c, chunksToCopy){
		OmMipChunkPtr chunk;
		vol->GetChunk(chunk, c);
		OmDataWrapperPtr dataPtrMapped = chunk->RawReadChunkDataUCHARmapped();
		vol->AllocInternalData(dataPtrMapped);
		break;
	}

	vol->copyDataIn(chunksToCopy);

	import_timer.stop();

	printf("done in %.2f secs\n",import_timer.s_elapsed());
	return true;
}

template <typename T>
void OmVolumeImporter<T>::addToChunkCoords(const OmMipChunkCoord chunk_coord)
{
	QMutexLocker lock(&mutex);
	chunksToCopy.insert(chunk_coord);
}
