#pragma once

class OmSimpleRawVol {
 public:
  template <typename VOL>
  static std::shared_ptr<QFile> Open(VOL* vol, const int mipLevel) {
    const std::string fname = OmFileNames::GetMemMapFileName(vol, mipLevel);

    std::shared_ptr<QFile> file =
        std::make_shared<QFile>(QString::fromStdString(fname));

    om::file::old::openFileRW(*file);

    return file;
  }
};
