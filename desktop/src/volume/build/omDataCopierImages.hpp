#pragma once

#include "volume/build/omDataCopierBase.hpp"
#include "volume/build/omLoadImage.h"
#include "volume/OmSimpleRawVol.hpp"

#include <QImage>

template <typename VOL>
class OmDataCopierImages : public OmDataCopierBase<VOL> {
private:
    VOL *const vol_;
    const std::vector<QFileInfo>& files_;

    std::vector<std::shared_ptr<QFile> > volFiles_;

public:
    OmDataCopierImages(VOL* vol, const std::vector<QFileInfo>& files)
        : OmDataCopierBase<VOL>(vol)
        , vol_(vol)
        , files_(files)
    {}

    void ReplaceSlice(const int sliceNum)
    {
        std::shared_ptr<QFile> mip0volFile = OmSimpleRawVol::Open(vol_, 0);

        const int depth = QImage(files_[0].absoluteFilePath()).depth();

        if(8 == depth)
        {
            OmLoadImage<VOL, uint8_t> imageLoader(vol_, mip0volFile, files_);
            imageLoader.ReplaceSlice(sliceNum);

        } else if(32 == depth)
        {
            OmLoadImage<VOL, uint32_t> imageLoader(vol_, mip0volFile, files_);
            imageLoader.ReplaceSlice(sliceNum);

        } else {
            throw om::IoException("don't know how to import image with bpp of");
        }
    }

protected:

    virtual void doImport()
    {
        const int depth = QImage(files_[0].absoluteFilePath()).depth();

        allocateData(determineDataType(depth));

        std::shared_ptr<QFile> mip0volFile = volFiles_[0];

        if(8 == depth)
        {
            OmLoadImage<VOL, uint8_t> imageLoader(vol_, mip0volFile, files_);
            imageLoader.Process();

        } else if(32 == depth)
        {
            OmLoadImage<VOL, uint32_t> imageLoader(vol_, mip0volFile, files_);
            imageLoader.Process();

        } else {
            throw om::IoException("don't know how to import image with bpp of");
        }
    }

    OmVolDataType determineDataType(const int depth)
    {
        switch(depth){
        case 8:
            return OmVolDataType::UINT8;
        case 32:
            return OmVolDataType::UINT32;
        default:
            throw om::IoException("don't know how to import image with bpp of");
        }
    }

    void allocateData(const OmVolDataType type){
        volFiles_ = OmVolumeAllocater::AllocateData(vol_, type);
    }
};

