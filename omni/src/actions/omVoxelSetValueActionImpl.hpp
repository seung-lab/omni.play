#ifndef OM_VOXEL_SET_VALUE_ACTION_IMPL_HPP
#define OM_VOXEL_SET_VALUE_ACTION_IMPL_HPP

#include "common/omCommon.h"
#include "project/omProject.h"
#include "common/omCommon.h"
#include "omVoxelSetValueAction.h"
#include "segment/omSegmentSelected.hpp"
#include "system/omStateManager.h"
#include "volume/omSegmentation.h"
#include "volume/omVolume.h"

class OmVoxelSetValueActionImpl {
private:
	//segmentation of voxels
	OmID mSegmentationId;

	//map of voxels to old values
	std::map<DataCoord, OmSegID> mOldVoxelValues;

	//new value of voxels
	OmSegID mNewValue;

public:
	OmVoxelSetValueActionImpl(const OmID segmentationId,
							  const DataCoord& rVoxel,
							  const OmSegID value)
	{
		//store segmentation id
		mSegmentationId = segmentationId;

		//store new value
		mNewValue = value;

		//store old value of voxel
		mOldVoxelValues[rVoxel] = mNewValue;
	}

	OmVoxelSetValueActionImpl(const OmID segmentationId,
							  const std::set<DataCoord>& rVoxels,
							  const OmSegID value)
	{
		//store segmentation id
		mSegmentationId = segmentationId;

		//store new value
		mNewValue = value;

		// TODO: fixme???? mNewValue == "old value" ??????
		//store old values of voxels
		FOR_EACH(itr, rVoxels){
			mOldVoxelValues[*itr] = mNewValue;
		}
	}

	void Execute()
	{
		//set voxel
		OmSegmentation & r_segmentation = OmProject::GetSegmentation(mSegmentationId);

		//modified voxels
		std::set<DataCoord> edited_voxels;

		FOR_EACH(itr, mOldVoxelValues){
			//set voxel to new value
			r_segmentation.SetVoxelValue(itr->first, mNewValue);
			edited_voxels.insert(itr->first);
		}
	}

	void Undo()
	{
		//set voxel
		OmSegmentation & r_segmentation = OmProject::GetSegmentation(mSegmentationId);

		//modified voxels
		std::set<DataCoord> edited_voxels;

		FOR_EACH(itr, mOldVoxelValues){
			//set voxel to prev value
			r_segmentation.SetVoxelValue(itr->first, itr->second);
			edited_voxels.insert(itr->first);
		}
	}

	std::string Description() const
	{
		std::string plurlize;
		if (mOldVoxelValues.size() > 1){
			plurlize = "s";
		}

		if(!mNewValue){
			return "Remove Voxel" + plurlize;
		} else {
			return "Set Voxel" + plurlize;
		}
	}

	QString classNameForLogFile() const {
		return "OmVolxelSetvalueAction";
	}

private:
	template <typename T> friend class OmActionLoggerFSThread;
	friend class QDataStream &operator<<(QDataStream&,
										 const OmVoxelSetValueActionImpl&);
	friend class QDataStream &operator>>(QDataStream&,
										 OmVoxelSetValueActionImpl&);

};

#endif
