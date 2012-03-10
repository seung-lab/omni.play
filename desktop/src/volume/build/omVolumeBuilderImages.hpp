#pragma once

#include "volume/build/omCompactVolValues.hpp"
#include "volume/build/omVolumeBuilderBase.hpp"
#include "volume/build/omDataCopierImages.hpp"

#include <QImage>

template <typename VOL>
class OmVolumeBuilderImages : public OmVolumeBuilderBase<VOL> {
private:
    VOL *const vol_;
    const std::vector<QFileInfo> files_;

public:
    OmVolumeBuilderImages(VOL* vol, const std::vector<QFileInfo> files)
        : OmVolumeBuilderBase<VOL>(vol)
        , vol_(vol)
        , files_(files)
    {}

private:
    virtual void importSourceData()
    {
        OmDataCopierImages<VOL> importer(vol_, files_);
        importer.Import();
    }

    virtual Vector3i getMip0Dims()
    {
        QImage img(files_[0].absoluteFilePath());
        const Vector3i dims(img.width(), img.height(), files_.size());
        std::cout << "dims are " << dims << "\n";
        return dims;
    }

    virtual void rewriteMip0Volume(OmSegmentation* vol)
    {
        printf("rewriting segment IDs...\n");

        OmCompactVolValues rewriter(vol);
        rewriter.Rewrite();

        printf("done!\n");
    }
};

