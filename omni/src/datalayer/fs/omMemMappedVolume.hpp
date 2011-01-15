#ifndef OM_MEM_MAPPED_VOLUME_HPP
#define OM_MEM_MAPPED_VOLUME_HPP

#include "datalayer/omIDataVolume.hpp"
#include "common/omDebug.h"
#include "datalayer/fs/omMemMappedFileQT.hpp"
#include "project/omProject.h"
#include "system/omProjectData.h"
#include "utility/omStringHelpers.h"
#include "volume/omMipChunkCoord.h"
#include "volume/omVolumeTypes.hpp"
#include "volume/omChannel.h"
#include "volume/omSegmentation.h"
#include "datalayer/omIDataVolume.hpp"
#include "datalayer/fs/omFileNames.hpp"

#include <zi/mutex.hpp>
#include <QFile>

template <typename T> class OmIOnDiskFile;

template <typename T, typename VOL>
class OmMemMappedVolume : public OmIDataVolume<T,VOL> {
private:
	VOL* vol_;
	std::vector<boost::shared_ptr<OmIOnDiskFile<T> > > maps_;

	typedef OmMemMappedFileReadQT<T> reader_t;
	typedef OmMemMappedFileWriteQT<T> writer_t;

public:
	OmMemMappedVolume(){} // for boost::varient
	OmMemMappedVolume(VOL* vol)
		: vol_(vol)
	{}

	OmRawDataPtrs GetType() const {
		return (T*)0;
	}

	void Load()
	{
		resizeMapsVector();

		for(size_t level = 0; level < maps_.size(); ++level) {
			maps_[level] = reader_t::Reader(getFileName(level));
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
					  << OmStringHelpers::CommaDeliminateNum(size)
					  << " (" << dims.x
					  << "," << dims.y
					  << "," << dims.z
					  << ")\n";

			maps_[level] =writer_t::WriterNumBytes(getFileName(level),
												   size,
												   om::DONT_ZERO_FILL);
		}

		printf("OmMemMappedVolume: done allocating data\n");
	}

	T* GetPtr(const int level) const {
		return maps_[level]->GetPtr();
	}

	T* GetChunkPtr(const OmMipChunkCoord& coord) const
	{
		const int level = coord.Level;
		const uint64_t offset = vol_->ComputeChunkPtrOffsetBytes(coord);
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

	std::string getFileName(const int level) const {
		return OmFileNames::GetMemMapFileName(vol_, level);
	}
};

#endif
