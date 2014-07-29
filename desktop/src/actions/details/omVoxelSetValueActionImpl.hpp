#pragma once
#include "precomp.h"

#include "common/common.h"
#include "omVoxelSetValueAction.h"
#include "segment/omSegmentSelected.hpp"
#include "segment/omSegments.h"
#include "segment/selection.hpp"
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
    for (auto& itr : rVoxels) {
      mOldVoxelValues[itr] = mNewValue;
    }
  }

  void Execute() {
    // set voxel
    SegmentationDataWrapper sdw(mSegmentationId);
    if (!sdw.IsValidWrapper()) {
      log_errors << "Unable to execute OmVolxelSetvalueAction";
      return;
    }
    auto system = sdw.GetSegmentation()->Coords();
    auto& chunkDS = sdw.GetSegmentation()->ChunkDS();
    auto selection = sdw.Segments()->Selection().GetSelectedSegmentIDs();

    std::unordered_map<om::coords::Chunk, std::set<om::coords::Global>> grouped;
    for (auto& iter : mOldVoxelValues) {
      grouped[iter.first.ToChunk(system, 0)].insert(iter.first);
    }

    for (auto& chunkPts : grouped) {
      auto& cc = chunkPts.first;
      auto& points = chunkPts.second;
      auto chunk = chunkDS.Get(cc);
      auto typedChunk = boost::get<om::chunk::Chunk<uint32_t>>(chunk.get());
      if (!typedChunk) {
        log_errors << "Unable to write to chunk " << cc;
        continue;
      }
      for (auto& point : points) {
        auto& ref = (*typedChunk)[point.ToData(system, 0).ToChunkOffset()];
        if (mNewValue || selection.count(ref)) {
          ref = mNewValue;
        }
      }
      chunkDS.Put(cc, chunk);
      sdw.GetSegmentation()->InvalidateTiles(cc);
    }
  }

  void Undo() {
    // set voxel
    SegmentationDataWrapper sdw(mSegmentationId);
    if (!sdw.IsValidWrapper()) {
      log_errors << "Unable to execute OmVolxelSetvalueAction";
      return;
    }

    for (auto& itr : mOldVoxelValues) {
      sdw.GetSegmentation()->SetVoxelValue(itr.first, itr.second);
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
