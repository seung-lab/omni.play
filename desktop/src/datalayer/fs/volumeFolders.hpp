#pragma once

#include "project/omProject.h"
#include "datalayer/file.h"

namespace om {
namespace fs {

struct volumeFolderInfo {
  om::file::path path;
  uint32_t id;
};

class VolumeFolders {
 public:
  static std::vector<volumeFolderInfo> FindSegmentations() {
    return find(OmProject::Paths().Segmentations(), "segmentation");
  }

  static std::vector<volumeFolderInfo> FindChannels() {
    return find(OmProject::Paths().Channels(), "channel");
  }

 private:
  static std::vector<volumeFolderInfo> find(const om::file::path folder,
                                            std::string toMatch) {
    std::vector<volumeFolderInfo> ret;
    if (!om::file::IsFolder(folder)) {
      return std::vector<volumeFolderInfo>();
    }

    for (auto& f : boost::filesystem::directory_iterator(folder)) {
      if (om::file::IsFolder(f)) {
        std::string dirName = f.path().filename().string();
        if (dirName.find(toMatch) != std::string::npos) {
          auto num = dirName.substr(toMatch.length());
          auto val = std::stoul(num);
          ret.push_back({f.path(), (uint32_t)val});

          log_debugs << "found volume " << f.path().string();
        }
      }
    }

    return ret;
  }
};

}  // namespace fs
}  // namespace om
