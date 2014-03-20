#pragma once
#include "precomp.h"

#include "common/common.h"
#include "omVoxelSetValueAction.h"
#include "segment/omSegmentSelected.hpp"
#include "system/omStateManager.h"
#include "volume/omSegmentation.h"
#include "utility/segmentationDataWrapper.hpp"

class OmVoxelSetValueActionImpl {
 private:
  // segmentation of voxels
  om::common::ID mSegmentationId;

  // map of voxels to old values
  std::map<om::coords::Global, om::common::SegID> mOldVoxelValues;

  // new value of voxels
  om::common::SegID mNewValue;

 public:
  OmVoxelSetValueActionImpl() {}

  OmVoxelSetValueActionImpl(const om::common::ID segmentationId,
                            const om::coords::Global& rVoxel,
                            const om::common::SegID value) {
    // store segmentation id
    mSegmentationId = segmentationId;

    // store new value
    mNewValue = value;

    // store old value of voxel
    mOldVoxelValues[rVoxel] = mNewValue;
  }

  OmVoxelSetValueActionImpl(const om::common::ID segmentationId,
                            const std::set<om::coords::Global>& rVoxels,
                            const om::common::SegID value) {
    // store segmentation id
    mSegmentationId = segmentationId;

    // store new value
    mNewValue = value;

    // TODO: fixme???? mNewValue == "old value" ??????
    // store old values of voxels
    FOR_EACH(itr, rVoxels) { mOldVoxelValues[*itr] = mNewValue; }
  }

  void Execute() {
    // set voxel
    SegmentationDataWrapper sdw(mSegmentationId);
    if (!sdw.IsValidWrapper()) {
      log_errors << "Unable to execute OmVolxelSetvalueAction";
      return;
    }

    // modified voxels
    std::set<om::coords::Global> edited_voxels;

    FOR_EACH(itr, mOldVoxelValues) {
      // set voxel to new value
      if (mNewValue == 0)  // erasing
      {
        if (sdw.GetSegmentation()->SetVoxelValueIfSelected(itr->first,
                                                           mNewValue)) {
          edited_voxels.insert(itr->first);
        }
      } else {
        sdw.GetSegmentation()->SetVoxelValue(itr->first, mNewValue);
        edited_voxels.insert(itr->first);
      }
    }
  }

  void Undo() {
    // set voxel
    SegmentationDataWrapper sdw(mSegmentationId);
    if (!sdw.IsValidWrapper()) {
      log_errors << "Unable to execute OmVolxelSetvalueAction";
      return;
    }

    // modified voxels
    std::set<om::coords::Global> edited_voxels;

    FOR_EACH(itr, mOldVoxelValues) {
      // set voxel to prev value
      sdw.GetSegmentation()->SetVoxelValue(itr->first, itr->second);
      edited_voxels.insert(itr->first);
    }
  }

  std::string Description() const {
    std::string plurlize;
    if (mOldVoxelValues.size() > 1) {
      plurlize = "s";
    }

    if (!mNewValue) {
      return "Remove Voxel" + plurlize;
    } else {
      return "Set Voxel" + plurlize;
    }
  }

  QString classNameForLogFile() const { return "OmVolxelSetvalueAction"; }

 private:
  template <typename T>
  friend class OmActionLoggerThread;
  friend class QDataStream& operator<<(QDataStream&,
                                       const OmVoxelSetValueActionImpl&);
  friend class QDataStream& operator>>(QDataStream&,
                                       OmVoxelSetValueActionImpl&);
};
