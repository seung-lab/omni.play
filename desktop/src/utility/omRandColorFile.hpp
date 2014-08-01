#pragma once
#include "precomp.h"

#include "common/colors.h"
#include "common/common.h"
#include "datalayer/paths.hpp"
#include "datalayer/memMappedFile.hpp"

namespace om {
namespace utility {
class RandColorFile {
 public:
  RandColorFile(const om::file::Paths& paths);
  ~RandColorFile() {}

  om::common::Color GetRandomColor();
  om::common::Color GetRandomColor(const uint32_t segID);

 private:
  void createOrLoad();
  void map();
  static void buildColorTable(std::vector<om::common::Color>& colorTable);
  void setupFile();

  static const int version_ = 1;

  const om::file::path fnp_;

  om::datalayer::MemMappedFile<om::common::Color> file_;
  om::common::Color* values_;
  int64_t numEntries_;

  friend class OmProjectGlobals;
};
}

namespace datalayer {
extern template class MemMappedFile<om::common::Color>;
}
}