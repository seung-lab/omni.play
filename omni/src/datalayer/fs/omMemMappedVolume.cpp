#include "common/omDebug.h"
#include "datalayer/fs/omMemMappedFileQT.hpp"
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
void OmMemMappedVolume<T,VOL>::resizeMapsVector()
{
	maps_.resize(vol_->GetRootMipLevel()+1);
}

template <typename T, typename VOL>
void OmMemMappedVolume<T,VOL>::Load()
{
	zi::Guard g(mutex_);

	resizeMapsVector();

	for(size_t level = 0; level < maps_.size(); ++level) {
		maps_[level] =
			boost::make_shared<OmMemMappedFileReadQT<T> >(getFileName(level),
														  0);
	}
}

template <typename T, typename VOL>
void OmMemMappedVolume<T,VOL>::Create(const std::map<int, Vector3i> & levelsAndDims)
{
	zi::Guard g(mutex_);

	resizeMapsVector();

	FOR_EACH(it, levelsAndDims){
		const int level = it->first;
		const Vector3i dims = it->second;
		const qint64 size =
			(qint64)dims.x
			*(qint64)dims.y
			*(qint64)dims.z
			*(qint64)GetBytesPerSample();

		assert(size);

		printf("mip %d: size is: %s (%dx%dx%d)\n",
		       level, qPrintable(StringHelpers::commaDeliminateNumber(size)),
		       dims.x, dims.y, dims.z);

		maps_[level] =
			boost::make_shared<OmMemMappedFileWriteQT<T> >(getFileName(level),
														   size);
	}

	printf("OmMemMappedVolume done allocating data\n");
}

template <typename T, typename VOL>
T* OmMemMappedVolume<T,VOL>::GetChunkPtr(const OmMipChunkCoord& coord) const
{
	const int level = coord.Level;
	const Vector3i dims = vol_->getDimsRoundedToNearestChunk(level);

	const qint64 x = (qint64)coord.getCoordinateX();
	const qint64 y = (qint64)coord.getCoordinateY();
	const qint64 z = (qint64)coord.getCoordinateZ();

	const qint64 xWidth  = 128;
	const qint64 yDepth  = 128;
	const qint64 zHeight = 128;

	const qint64 slabSize = (qint64)dims.x * (qint64)dims.y * (qint64)zHeight * (qint64)GetBytesPerSample();
	const qint64 rowSize =  (qint64)dims.x * (qint64)yDepth  * (qint64)zHeight * (qint64)GetBytesPerSample();
	const qint64 cSize =    (qint64)xWidth  * (qint64)yDepth  * (qint64)zHeight * (qint64)GetBytesPerSample();

	const qint64 offset = slabSize*z + rowSize*y + cSize*x;

	debug("newimport", "offset is: %llu (%d,%d,%d) for (%d,%d,%d)\n", offset,
	      DEBUGV3(dims), DEBUGV3(coord.Coordinate));

	T* ret = maps_[level]->GetPtrWithOffset(offset);
	assert(ret);
	return ret;
}

template <typename T, typename VOL>
std::string OmMemMappedVolume<T,VOL>::getFileName(const int level) const
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

	return fnp.toStdString();
}
