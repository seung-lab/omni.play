#pragma once

class OmSimpleRawVol {
 public:
  template <typename VOL>
  static om::shared_ptr<QFile> Open(VOL* vol, const int mipLevel) {
    const std::string fname = OmFileNames::GetMemMapFileName(vol, mipLevel);

    om::shared_ptr<QFile> file =
        om::make_shared<QFile>(QString::fromStdString(fname));

    om::file::openFileRW(*file);

    return file;
  }
};
