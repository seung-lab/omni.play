#include "common/omDebug.h"
#include "system/omProjectData.h"
#include "utility/stringHelpers.h"
#include "project/omProject.h"
#include "datalayer/fs/omMemMappedVolume.hpp"

template <typename T, typename VOL>
OmMemMappedVolume<T,VOL>::OmMemMappedVolume()
	: vol_(NULL)
{
	alreadyAllocFiles = false;
}

template <typename T, typename VOL>
OmMemMappedVolume<T,VOL>::~OmMemMappedVolume()
{
	foreach(QFile * f, mFileVec){
		delete f;
		f = NULL;
	}
}

template <typename T, typename VOL>
void OmMemMappedVolume<T,VOL>::AllocMemMapFiles(VOL* invol)
{
	zi::Guard g(mutex_);

	vol_ = invol;
	assert(vol_);

	mFileVec.resize(vol_->GetRootMipLevel()+1);
	mFileMapPtr.resize(vol_->GetRootMipLevel()+1);

	for (int level = 0; level <= vol_->GetRootMipLevel(); level++) {

		Vector3i data_dims = vol_->MipLevelDataDimensions(level);

		//round up to nearest chunk
		Vector3i rdims =
			Vector3i(ROUNDUP(data_dims.x, vol_->GetChunkDimension()),
				 ROUNDUP(data_dims.y, vol_->GetChunkDimension()),
				 ROUNDUP(data_dims.z, vol_->GetChunkDimension()));

		const qint64 size = (qint64)rdims.x
			*(qint64)rdims.y
			*(qint64)rdims.z
			*(qint64)GetBytesPerSample();

		assert(size);

		printf("mip %d: size is: %s (%dx%dx%d)\n",
		       level, qPrintable(StringHelpers::commaDeliminateNumber(size)),
		       rdims.x, rdims.y, rdims.z);

		const QString fn=QString("%1_%2_mip%3_%4bit.raw")
			.arg(OmProject::GetFileName().replace(".omni",""))
			.arg(vol_->GetDirectoryPath().replace("channels/","").replace("segmentations/","").replace("/",""))
			.arg(level)
			.arg(8*GetBytesPerSample());

		const QString fnp = OmProjectData::getAbsolutePath()+"/"+fn;
		QFile* file = mFileVec[level] = new QFile(fnp);
		file->remove();
		if(!file->open(QIODevice::ReadWrite)){
			printf("could not create chunk file %s\n", qPrintable(fnp));
			assert(0);
		}
		file->resize(size);
		/*
		printf("\tpre-allocating...\n");
		for( qint64 i=0; i < size; i+=(qint64)4096){
			file->seek(i);
			file->putChar(0);
		}
		printf("\tflushing...\n");
		file->flush();
		*/
		mFileMapPtr[level] = file->map(0,size);
		file->close();
	}

	alreadyAllocFiles = true;
}

template <typename T, typename VOL>
T* OmMemMappedVolume<T,VOL>::getChunkPtr( OmMipChunkCoord & coord)
{
	const int level = coord.getLevel();
	Vector3 < int >data_dims = vol_->MipLevelDataDimensions(level);

	//round up to nearest chunk
	Vector3i rdims =
		Vector3i(ROUNDUP(data_dims.x, vol_->GetChunkDimension()),
			 ROUNDUP(data_dims.y, vol_->GetChunkDimension()),
			 ROUNDUP(data_dims.z, vol_->GetChunkDimension()));

	const qint64 x = (qint64)coord.getCoordinateX();
	const qint64 y = (qint64)coord.getCoordinateY();
	const qint64 z = (qint64)coord.getCoordinateZ();

	const qint64 xWidth  = 128;
	const qint64 yDepth  = 128;
	const qint64 zHeight = 128;

	const qint64 slabSize = (qint64)rdims.x * (qint64)rdims.y * (qint64)zHeight * (qint64)GetBytesPerSample();
	const qint64 rowSize =  (qint64)rdims.x * (qint64)yDepth  * (qint64)zHeight * (qint64)GetBytesPerSample();
	const qint64 cSize =    (qint64)xWidth  * (qint64)yDepth  * (qint64)zHeight * (qint64)GetBytesPerSample();

	const qint64 offset = slabSize*z + rowSize*y + cSize*x;

	debug("newimport", "offset is: %llu (%d,%d,%d) for (%d,%d,%d)\n", offset,
	      DEBUGV3(rdims), DEBUGV3(coord.Coordinate));

	return (T*)(mFileMapPtr[level]+offset);
}
