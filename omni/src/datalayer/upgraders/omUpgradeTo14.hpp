#ifndef OM_UPGRADE_TO_14_HPP
#define OM_UPGRADE_TO_14_HPP

#include "project/omProject.h"
#include "volume/omVolumeData.hpp"
#include "volume/omMipVolume.h"
#include "volume/omVolumeTypes.hpp"

// extract volumes from hdf5 to mem-map

class OmUpgradeTo14{
public:
	void copyDataOutFromHDF5(){
		doConvert();
	}

private:

	void doConvert()
	{
		FOR_EACH(iter, OmProject::GetValidChannelIds()){
			OmChannel& channel = OmProject::GetChannel(*iter);
			printf("converting channel %d\n", *iter);
			convertVolume(channel);
		}

		FOR_EACH(iter, OmProject::GetValidSegmentationIds()){
			OmSegmentation& segmentation = OmProject::GetSegmentation(*iter);
			printf("converting segmentation %d\n", *iter);
			convertVolume(segmentation);
		}
	}

	template <typename T>
	void convertVolume(T& vol)
	{
		allocate(vol);
		copyData(vol);
	}

	template <typename T>
	void allocate(T& vol)
	{
		std::map<int, Vector3i> levelsAndDims;

		for (int level = 0; level <= vol.GetRootMipLevel(); level++) {
			levelsAndDims[level] = vol.getDimsRoundedToNearestChunk(level);
		}

		// allocate mem-mapped files...
		vol.getVolData()->create(&vol, levelsAndDims);
	}

	template <typename T>
	void copyData(T& vol)
	{
		vol.copyAllMipDataIntoMemMap();
	}

};

#endif
