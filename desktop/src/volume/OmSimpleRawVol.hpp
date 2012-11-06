#pragma once

class OmSimpleRawVol {
public:
    template <typename VOL>
    static boost::shared_ptr<QFile> Open(VOL* vol, const int mipLevel)
    {
        const std::string fname = OmFileNames::GetMemMapFileName(vol, mipLevel);

        boost::shared_ptr<QFile> file = boost::make_shared<QFile>(QString::fromStdString(fname));

        om::file::openFileRW(*file);

        return file;
    }
};
