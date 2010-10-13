#ifndef OM_MEM_MAPPED_VOLUME_HPP
#define OM_MEM_MAPPED_VOLUME_HPP

#include "datalayer/omIDataVolume.hpp"
#include "common/omDebug.h"
#include "datalayer/fs/omMemMappedFileQT.hpp"
#include "project/omProject.h"
#include "system/omProjectData.h"
#include "utility/stringHelpers.h"
#include "volume/omMipChunkCoord.h"
#include "volume/omVolumeTypes.hpp"
#include "volume/omChannel.h"
#include "volume/omSegmentation.h"
#include "datalayer/omIDataVolume.hpp"

#include <zi/mutex.hpp>
#include <QFile>

template <typename T> class OmIMemMappedFile;

template <typename T, typename VOL>
class OmMemMappedVolume : public OmIDataVolume<T,VOL> {
private:
	VOL* vol_;
	std::vector<boost::shared_ptr<OmIMemMappedFile<T> > > maps_;

	typedef OmMemMappedFileReadQT<T> reader_t;
	typedef OmMemMappedFileWriteQT<T> writer_t;

public:
	OmMemMappedVolume(){} // for boost::varient
	OmMemMappedVolume(VOL* vol)
		: vol_(vol)
	{}

	void Load()
	{
		resizeMapsVector();

		for(size_t level = 0; level < maps_.size(); ++level) {
			maps_[level] =
				boost::make_shared<reader_t>(getFileName(level),
							     0);
		}
	}

	void Create(const std::map<int, Vector3i>& levelsAndDims)
	{
		resizeMapsVector();

		const int64_t bps = GetBytesPerSample();

		FOR_EACH(it, levelsAndDims){
			const int level = it->first;
			const Vector3<int64_t> dims = it->second;
			const int64_t size = dims.x * dims.y * dims.z * bps;

			std::cout << "mip " << level << ": size is: "
					  << StringHelpers::commaDeliminateNum(size)
					  << " (" << dims.x
					  << "," << dims.y
					  << "," << dims.z
					  << ")\n";

			maps_[level] =
				boost::make_shared<writer_t>(getFileName(level),
							     size);
		}

		printf("OmMemMappedVolume: done allocating data\n");
	}

	T* GetPtr(const int level) const {
		return maps_[level]->GetPtr();
	}

	T* GetChunkPtr(const OmMipChunkCoord& coord) const
	{
		const int level = coord.Level;
		const Vector3<int64_t> volDims = vol_->getDimsRoundedToNearestChunk(level);
		const Vector3<int64_t> chunkDims = vol_->GetChunkDimensions();
		const int64_t bps = GetBytesPerSample();

		const int64_t slabSize  = volDims.x   * volDims.y   * chunkDims.z * bps;
		const int64_t rowSize   = volDims.x   * chunkDims.y * chunkDims.z * bps;
		const int64_t chunkSize = chunkDims.x * chunkDims.y * chunkDims.z * bps;

		const Vector3<int64_t> chunkPos = coord.Coordinate; // bottom left corner
		const int64_t offset =
			slabSize*chunkPos.z + rowSize*chunkPos.y + chunkSize*chunkPos.x;

		ZiLOG(DEBUG, io) << "offset is: " << offset
						 << " (" << volDims << ") for "
						 << coord.Coordinate << "\n";

		T* ret = maps_[level]->GetPtrWithOffset(offset);
		assert(ret);
		return ret;
	}

	int GetBytesPerSample() const {
		return sizeof(T);
	}

private:

	void resizeMapsVector(){
		maps_.resize(vol_->GetRootMipLevel() + 1);
	}

	//TODO: cleanup!
	//ex:  /home/projectName.files/segmentations/segmentation1/0/volume.int32_t.raw
	std::string getFileName(const int level) const
	{
		const QDir filesDir = OmProjectData::GetFilesFolderPath();

		const QString subPath = QString("%1/%2/")
			.arg(QString::fromStdString(vol_->GetDirectoryPath()))
			.arg(level);

		if(subPath.startsWith("/")){
			throw OmIoException("not a relative path: " + subPath.toStdString());
		}

		const QString fullPath = filesDir.absolutePath() + QDir::separator() + subPath;

		if(!filesDir.mkpath(subPath)){
			throw OmIoException("could not create folder " + fullPath.toStdString());
		}

		const std::string volType =vol_->getVolDataTypeAsStr();

		const QString fnp = QString("/%1/volume.%2.raw")
			.arg(fullPath)
			.arg(QString::fromStdString(volType));

		const QString fnp_clean = QDir::cleanPath(fnp);

		ZiLOG(DEBUG, io) << "file is " << fnp_clean.toStdString() << "\n";

		return fnp_clean.toStdString();
	}
};

#endif
