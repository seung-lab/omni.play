#include "common/omDebug.h"
#include "datalayer/fs/omMemMappedVolume.hpp"
#include "project/omProject.h"
#include "system/omProjectData.h"
#include "utility/stringHelpers.h"
#include "volume/omMipChunkCoord.h"
#include "volume/omVolumeTypes.hpp"

template <typename T, typename VOL>
OmMemMappedVolume<T,VOL>::OmMemMappedVolume(VOL* vol)
	: vol_(vol)
{
}

template <typename T, typename VOL>
OmMemMappedVolume<T,VOL>::~OmMemMappedVolume()
{
	for(size_t level = 0; level < mFileVec.size(); ++level) {
		QFile * f = mFileVec[level];
		printf("closing file %s\n", qPrintable(f->fileName()));
		delete f;
	}
}

template <typename T, typename VOL>
void OmMemMappedVolume<T,VOL>::load()
{
	zi::Guard g(mutex_);

	mFileVec.resize(vol_->GetRootMipLevel()+1);
	mFileMapPtr.resize(vol_->GetRootMipLevel()+1);

	for(size_t level = 0; level < mFileVec.size(); ++level) {
		openAndmMemMap(level);
	}
}

template <typename T, typename VOL>
void OmMemMappedVolume<T,VOL>::create(const std::map<int, Vector3i> & levelsAndDims)
{
	zi::Guard g(mutex_);

	mFileVec.resize(levelsAndDims.size());
	mFileMapPtr.resize(levelsAndDims.size());

	FOR_EACH(it, levelsAndDims){
		const int level = it->first;
		const Vector3i rdims = it->second;
		const qint64 size = (qint64)rdims.x
			*(qint64)rdims.y
			*(qint64)rdims.z
			*(qint64)GetBytesPerSample();

		assert(size);

		printf("mip %d: size is: %s (%dx%dx%d)\n",
		       level, qPrintable(StringHelpers::commaDeliminateNumber(size)),
		       rdims.x, rdims.y, rdims.z);

		QFile::remove(getFileName(level));
		QFile * file = openFile(level);
		file->resize(size);
		//allocateSpace(file);
		memMap(file, level);
	}

	printf("OmMemMappedVolume done allocating data\n");
}

template <typename T, typename VOL>
T* OmMemMappedVolume<T,VOL>::getChunkPtr(const OmMipChunkCoord & coord)
{
	const int level = coord.Level;
	const Vector3i rdims = vol_->getDimsRoundedToNearestChunk(level);

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

	T* ret = (T*)(mFileMapPtr.at(level)+offset);
	assert(ret);
	return ret;
}

template <typename T, typename VOL>
QString OmMemMappedVolume<T,VOL>::getFileName(const int level)
{
	const QString volName = QString::fromStdString(vol_->GetName());
	const QString volType =
		QString::fromStdString(OmVolumeTypeHelpers::GetTypeAsString(vol_->getVolDataType()));

	const QString fn=QString("%1--%2--mip%3--%4.raw")
		.arg(OmProject::GetFileName().replace(".omni",""))
		.arg(volName)
		.arg(level)
		.arg(volType);

	const QString fnp = OmProjectData::getAbsolutePath()+"/"+fn;

	return fnp;
}

template <typename T, typename VOL>
QFile* OmMemMappedVolume<T,VOL>::openFile(const int level)
{
	const QString fnp = getFileName(level);
	QFile* file = mFileVec[level] = new QFile(fnp);
	if(!file->open(QIODevice::ReadWrite)){
		printf("could not open chunk file %s\n", qPrintable(fnp));
		assert(0);
	}

	return file;
}

template <typename T, typename VOL>
void OmMemMappedVolume<T,VOL>::memMap(QFile * file, const int level)
{
	mFileMapPtr[level] = file->map(0,file->size());
	file->close();
}

template <typename T, typename VOL>
void OmMemMappedVolume<T,VOL>::openAndmMemMap(const int level)
{
	memMap(openFile(level), level);
}

template <typename T, typename VOL>
void OmMemMappedVolume<T,VOL>::allocateSpace(QFile * file)
{
	printf("\tpre-allocating...\n");
	for( qint64 i=0; i < file->size(); i+=(qint64)4096){
		file->seek(i);
		file->putChar(0);
	}
	printf("\tflushing...\n");
	file->flush();
}
