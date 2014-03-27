#pragma once
#include "precomp.h"

#include "datalayer/fs/omFile.hpp"

class OmSimpleRawVol {
 public:
  template <typename VOL>
  static std::shared_ptr<QFile> Open(VOL* vol, const int mipLevel) {
    const std::string fname =
        vol->VolPaths().Data(mipLevel, vol->getVolDataType()).string();

    std::shared_ptr<QFile> file =
        std::make_shared<QFile>(QString::fromStdString(fname));

    om::file::old::openFileRW(*file);

    return file;
  }
};
