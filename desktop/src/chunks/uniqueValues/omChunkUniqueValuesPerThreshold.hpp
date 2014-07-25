#pragma once
#include "precomp.h"

#include "utility/malloc.hpp"
#include "common/common.h"
#include "chunk/chunk.hpp"
#include "chunks/uniqueValues/omChunkUniqueValuesTypes.h"

#include "utility/image/omImage.hpp"
#include "utility/segmentationDataWrapper.hpp"
#include "volume/omSegmentation.h"
#include "segment/omSegments.h"

class OmChunkUniqueValuesPerThreshold {
 private:
  OmSegmentation& segmentation_;
  const om::coords::Chunk coord_;
  const double threshold_;
  const QString fnp_;

  std::shared_ptr<uint32_t> values_;
  size_t numElements_;

  zi::rwmutex mutex_;

 public:
  OmChunkUniqueValuesPerThreshold(OmSegmentation& segmentation,
                                  const om::coords::Chunk& coord,
                                  const double threshold)
      : segmentation_(segmentation),
        coord_(coord),
        threshold_(threshold),
        fnp_(filePath()),
        numElements_(0) {}

  ChunkUniqueValues Values() {
    zi::rwmutex::write_guard g(mutex_);

    if (!values_) {
      load();
    }

    return ChunkUniqueValues(values_, numElements_);
  }

  ChunkUniqueValues RereadChunk() {
    zi::rwmutex::write_guard g(mutex_);

    findValues();

    return ChunkUniqueValues(values_, numElements_);
  }

 private:
  void load() {
    QFile file(fnp_);
    if (!file.exists()) {
      findValues();
      return;
    }

    if (!file.open(QIODevice::ReadOnly)) {
      throw om::IoException("could not open");
    }

    values_ = om::mem::Malloc<uint32_t>::NumBytes(file.size(),
                                                  om::mem::ZeroFill::DONT);
    numElements_ = file.size() / sizeof(uint32_t);

    file.seek(0);

    char* data = reinterpret_cast<char*>(values_.get());
    file.read(data, file.size());
  }

  void findValues() {
    auto chunk = segmentation_.GetChunk(coord_);
    auto* typedChunk = boost::get<om::chunk::Chunk<uint32_t>>(chunk.get());
    if (!typedChunk) {
      log_errors << "Unable to get Segmetation Chunk.";
      return;
    }

    uint32_t const* const rawData = typedChunk->data().get();

    std::unordered_set<uint32_t> segIDs;

    if (!qFuzzyCompare(1, threshold_)) {
      auto& segments = segmentation_.Segments();
      segmentation_.SetDendThreshold(threshold_);

      for (size_t i = 0; i < typedChunk->length(); ++i) {
        if (0 != rawData[i]) {
          segIDs.insert(segments.FindRootID(rawData[i]));
        }
      }
    } else {
      for (size_t i = 0; i < typedChunk->length(); ++i) {
        if (0 != rawData[i]) {
          segIDs.insert(rawData[i]);
        }
      }
    }

    values_ = om::mem::Malloc<uint32_t>::NumElements(segIDs.size(),
                                                     om::mem::ZeroFill::DONT);

    std::copy(segIDs.begin(), segIDs.end(), values_.get());
    zi::sort(values_.get(), values_.get() + segIDs.size());

    numElements_ = segIDs.size();

    log_infos << "ChunkUniqueValues: chunk " << coord_ << " has "
              << numElements_ << " unique values";

    store();
  }

  void store() {
    QFile file(fnp_);

    if (!file.open(QIODevice::WriteOnly)) {
      throw om::IoException("could not open");
    }

    const int64_t numBytes = numElements_ * sizeof(uint32_t);
    file.resize(numBytes);

    const char* data = reinterpret_cast<const char*>(values_.get());

    file.write(data, numBytes);
  }

  QString filePath() {
    auto path = segmentation_.SegPaths().ChunkUniqueValues(coord_);

    if (!om::file::exists(path.parent_path())) {
      om::file::MkDir(path.parent_path());
    }

    return path.c_str();
  }
};
