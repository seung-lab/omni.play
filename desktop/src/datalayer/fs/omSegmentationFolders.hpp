#pragma once

#include "utility/omStringHelpers.h"

namespace om {
namespace fs {

struct segmentationFolderInfo {
  QString path;
  uint32_t id;
};

class segmentationFolders {
 public:

  static std::vector<segmentationFolderInfo> Find() {
    const QString folder = OmFileNames::FilesFolder() + "/segmentations/";
    QDir dir(folder);

    QStringList filters;
    filters << "segmentation*";
    dir.setNameFilters(filters);

    const QStringList dirNames = dir.entryList(QDir::Dirs);

    std::vector<segmentationFolderInfo> ret;

    FOR_EACH(iter, dirNames) {
      QString str = *iter;
      str.replace("segmentation", "");

      const uint32_t num = OmStringHelpers::getUInt(str);

      segmentationFolderInfo info = { folder + *iter, num };

      ret.push_back(info);

      std::cout << "found segmentation " << num << "\n";
    }

    return ret;
  }
};

}  // namespace fs
}  // namespace om
