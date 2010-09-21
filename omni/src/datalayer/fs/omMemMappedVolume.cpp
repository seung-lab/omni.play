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
	maps_.resize(vol_->GetRootMipLevel() + 1);
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
		const Vector3<int64_t> dims = it->second;
		const int64_t size =
			dims.x * dims.y * dims.z * (int64_t)GetBytesPerSample();

		printf("mip %d: size is: %s (%ldx%ldx%ld)\n",
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
	const Vector3<int64_t> volDims = vol_->getDimsRoundedToNearestChunk(level);
	const Vector3<int64_t> chunkDims(128, 128, 128);
	const int64_t bps = GetBytesPerSample();

	const int64_t slabSize  = volDims.x   * volDims.y   * chunkDims.z * bps;
	const int64_t rowSize   = volDims.x   * chunkDims.y * chunkDims.z * bps;
	const int64_t chunkSize = chunkDims.x * chunkDims.y * chunkDims.z * bps;

	const Vector3<int64_t> chunkPos = coord.Coordinate; // bottom left corner
	const int64_t offset =
		slabSize*chunkPos.z + rowSize*chunkPos.y + chunkSize*chunkPos.x;

	debug("memmap", "offset is: %llu (%ld,%ld,%ld) for (%ld,%ld,%ld)\n",
		  offset, DEBUGV3(volDims), DEBUGV3(coord.Coordinate));

	T* ret = maps_[level]->GetPtrWithOffset(offset);
	assert(ret);
	return ret;
}

template <typename T, typename VOL>
std::string OmMemMappedVolume<T,VOL>::getFileName(const int level) const
{
	const std::string volName = vol_->GetName();
	const std::string volType =
		OmVolumeTypeHelpers::GetTypeAsString(vol_->getVolDataType());

	const QString fn=QString("%1--%2--mip%3--%4.raw")
		.arg(OmProject::GetFileName().replace(".omni",""))
		.arg(QString::fromStdString(volName))
		.arg(level)
		.arg(QString::fromStdString(volType));

	const QString fnp = OmProjectData::getAbsolutePath()+"/"+fn;

	return fnp.toStdString();
}
