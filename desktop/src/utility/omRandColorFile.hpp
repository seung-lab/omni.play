#pragma once
#include "precomp.h"

#include "common/colors.h"
#include "common/common.h"
#include "common/string.hpp"
#include "datalayer/fs/omFile.hpp"
#include "datalayer/paths.hpp"
#include "utility/omRand.hpp"

class OmRandColorFile {
 private:
  static const int version_ = 1;

  const std::string fnp_;

  std::shared_ptr<QFile> file_;
  om::common::Color* values_;
  int64_t numEntries_;

  friend class OmProjectGlobals;

 public:
  OmRandColorFile(const om::file::Paths& paths)
      : fnp_(paths.RandColors().string()), values_(nullptr), numEntries_(0) {}

  ~OmRandColorFile() {}

  om::common::Color GetRandomColor() {
    assert(values_);
    const int index = OmRand::GetRandomInt(0, numEntries_ - 1);
    return values_[index];
  }

  om::common::Color GetRandomColor(const uint32_t segID) {
    assert(values_);
    const int index = segID % numEntries_;
    return values_[index];
  }

 private:
  void createOrLoad() {
    if (!om::file::old::exists(fnp_)) {
      setupFile();
    }
    mapReadOnly();
  }

  void mapReadOnly() {
    if (!om::file::old::exists(fnp_)) {
      throw om::IoException("file doesn't exist", fnp_);
    }

    om::file::old::openFileRO(file_, fnp_);

    values_ = om::file::old::mapFile<om::common::Color>(file_.get());

    numEntries_ = file_->size() / sizeof(om::common::Color);
  }

  static void buildColorTable(std::vector<om::common::Color>& colorTable) {
    // make sure to change version_ if color table algorithm changes...

    static const int min_variance = 120;

    colorTable.clear();
    colorTable.reserve(1952449);  // 1,952,448 entries for min_var = 120

    for (int r = 0; r < 128; ++r) {
      for (int g = 0; g < 128; ++g) {
        for (int b = 0; b < 128; ++b) {

          const int avg = (r + g + b) / 3;
          const int avg2 = (r * r + g * g + b * b) / 3;
          const int v = avg2 - avg * avg;

          if (v >= min_variance) {
            const om::common::Color color = {static_cast<uint8_t>(r),
                                             static_cast<uint8_t>(g),
                                             static_cast<uint8_t>(b)};
            colorTable.push_back(color);
          }
        }
      }
    }

    zi::random_shuffle(colorTable.begin(), colorTable.end());
  }

  void setupFile() {
    std::vector<om::common::Color> colorTable;

    buildColorTable(colorTable);

    QFile file(QString::fromStdString(fnp_));

    om::file::old::openFileRW(file);

    om::file::old::writeVec(file, colorTable);
  }
};
