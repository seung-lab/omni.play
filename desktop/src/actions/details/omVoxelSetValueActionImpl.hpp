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
 public:
  OmVoxelSetValueActionImpl() {}

  OmVoxelSetValueActionImpl(const om::common::ID segmentationId,
                            const std::set<om::coords::Data>& rVoxels,
                            const om::common::SegID value) {
    // store segmentation id
    segmentationID_ = segmentationId;

    // store new value
    newSegmentID_ = value;

    for (auto& itr : rVoxels) {
      oldVoxels_[itr] = 0;
    }
  }

  void Execute() {
    // set voxel
    SegmentationDataWrapper sdw(segmentationID_);
    if (!sdw.IsValidWrapper()) {
      log_errors << "Unable to execute OmVolxelSetvalueAction";
      return;
    }
    auto& chunkDS = sdw.GetSegmentation()->ChunkDS();
    auto selection = sdw.Segments()->Selection().GetSelectedSegmentIDs();

    std::unordered_map<om::coords::Chunk, std::set<om::coords::Data>> grouped;
    for (auto& iter : oldVoxels_) {
      grouped[iter.first.ToChunk()].insert(iter.first);
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
        auto& ref = (*typedChunk)[point.ToChunkOffset()];
        if (ref == newSegmentID_) {
          continue;
        }
        oldVoxels_[point] = ref;
        if (newSegmentID_ || selection.count(ref)) {
          ref = newSegmentID_;
        }
      }
      chunkDS.Put(cc, chunk);
      sdw.GetSegmentation()->InvalidateTiles(cc);
    }
  }

  void Undo() {
    // set voxel
    SegmentationDataWrapper sdw(segmentationID_);
    if (!sdw.IsValidWrapper()) {
      log_errors << "Unable to execute OmVolxelSetvalueAction";
      return;
    }
    auto& chunkDS = sdw.GetSegmentation()->ChunkDS();
    auto selection = sdw.Segments()->Selection().GetSelectedSegmentIDs();

    std::unordered_map<om::coords::Chunk, std::set<om::coords::Data>> grouped;
    for (auto& iter : oldVoxels_) {
      grouped[iter.first.ToChunk()].insert(iter.first);
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
        auto& ref = (*typedChunk)[point.ToChunkOffset()];
        if (newSegmentID_ || selection.count(ref)) {
          ref = oldVoxels_[point];
        }
      }
      chunkDS.Put(cc, chunk);
      sdw.GetSegmentation()->InvalidateTiles(cc);
    }
  }

  std::string Description() const {
    std::string plurlize;
    if (oldVoxels_.size() > 1) {
      plurlize = "s";
    }

    if (!newSegmentID_) {
      return "Remove Voxel" + plurlize;
    } else {
      return "Set Voxel" + plurlize;
    }
  }

  QString classNameForLogFile() const { return "OmVolxelSetvalueAction"; }

 private:
  om::common::ID segmentationID_;
  std::map<om::coords::Data, om::common::SegID> oldVoxels_;
  om::common::SegID newSegmentID_;

  template <typename T>
  friend class OmActionLoggerThread;
  friend class QDataStream& operator<<(QDataStream&,
                                       const OmVoxelSetValueActionImpl&);
  friend class QDataStream& operator>>(QDataStream&,
                                       OmVoxelSetValueActionImpl&);
};
