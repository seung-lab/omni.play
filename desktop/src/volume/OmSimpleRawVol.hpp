#pragma once
#include "precomp.h"

#include "datalayer/file.h"

class OmSimpleRawVol {
 public:
  template <typename VOL>
  static std::shared_ptr<QFile> Open(VOL* vol, const int mipLevel) {
    const std::string fname =
        vol->VolPaths().Data(mipLevel, vol->getVolDataType()).string();

    std::shared_ptr<QFile> file =
        std::make_shared<QFile>(QString::fromStdString(fname));

    if (!file->open(QIODevice::ReadWrite)) {
      throw om::IoException("could not open file read/write");
    }

    return file;
  }
};
