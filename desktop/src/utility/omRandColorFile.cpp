#include "utility/omRandColorFile.hpp"
#include "utility/omRand.hpp"

namespace om {
namespace utility {
RandColorFile::RandColorFile(const file::Paths& paths)
    : fnp_(paths.RandColors().string()), values_(nullptr), numEntries_(0) {
  createOrLoad();
}

common::Color RandColorFile::GetRandomColor() {
  assert(values_);
  const int index = OmRand::GetRandomInt(0, numEntries_ - 1);
  return values_[index];
}

common::Color RandColorFile::GetRandomColor(const uint32_t segID) {
  assert(values_);
  const int index = segID % numEntries_;
  return values_[index];
}

void RandColorFile::createOrLoad() {
  if (file::exists(fnp_)) {
    map();
  } else {
    setupFile();
  }
}

void RandColorFile::map() {
  file_ = datalayer::MemMappedFile<common::Color>(fnp_);
  values_ = file_.GetPtr();
}

void RandColorFile::buildColorTable(std::vector<common::Color>& colorTable) {
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
          const common::Color color = {static_cast<uint8_t>(r),
                                       static_cast<uint8_t>(g),
                                       static_cast<uint8_t>(b)};
          colorTable.push_back(color);
        }
      }
    }
  }

  zi::random_shuffle(colorTable.begin(), colorTable.end());
}

void RandColorFile::setupFile() {
  std::vector<common::Color> colorTable;

  buildColorTable(colorTable);
  file_ =
      datalayer::MemMappedFile<common::Color>::CreateFromData(fnp_, colorTable);
  values_ = file_.GetPtr();
}
}
}

namespace om {
namespace datalayer {
template class MemMappedFile<common::Color>;
}
}