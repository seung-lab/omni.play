#pragma once

class OmSimpleRawVol {
public:
    template <typename VOL>
    static boost::shared_ptr<QFile> Open(VOL* vol, const int mipLevel)
    {
        const std::string fname = fileNames::GetMemMapFileName(vol, mipLevel);

        boost::shared_ptr<QFile> file = om::make_shared<QFile>(std::string::fromStdString(fname));

        om::file::openFileRW(*file);

        return file;
    }
};
