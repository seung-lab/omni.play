#pragma once

#include "common/common.h"
#include "volume/omMipVolume.h"
#include "utility/omStringHelpers.h"
#include "datalayer/fs/omFileNames.hpp"

#include <QFile>

class OmVolumeAllocater {
public:
    static std::vector<boost::shared_ptr<QFile> >
    AllocateData(OmMipVolume* vol, const om::common::DataType type)
    {
        assert(om::common::DataType::UNKNOWN != type.index());
        vol->SetVolDataType(type);

        const int maxLevel = vol->Coords().RootMipLevel();

        std::vector<boost::shared_ptr<QFile> > volFiles(maxLevel + 1);

        for (int level = 0; level <= maxLevel; ++level)
        {
            const Vector3<uint64_t> dims =
                vol->Coords().DimsRoundedToNearestChunk(level);

            volFiles[level] = createFile(vol, level, dims);
        }

        printf("\tdone allocating volume for all mip levels; data type is %s\n",
               OmVolumeTypeHelpers::GetTypeAsString(type).c_str());

        vol->VolData()->load(vol);
        std::cout << "volumes memory mapped\n";

        return volFiles;
    }

    static void ReAllocateDownsampledVolumes(OmMipVolume* vol)
    {
        const int maxLevel = vol->Coords().RootMipLevel();

        std::vector<boost::shared_ptr<QFile> > volFiles(maxLevel + 1);

        for(int level = 1; level <= maxLevel; ++level)
        {
            const Vector3<uint64_t> dims =
                vol->Coords().DimsRoundedToNearestChunk(level);

            volFiles[level] = createFile(vol, level, dims);
        }

        printf("\tdone reallocating volume for all mip levels\n");

        vol->VolData()->load(vol);
        std::cout << "volumes memory mapped\n";
    }

private:

    static boost::shared_ptr<QFile>
    createFile(OmMipVolume* vol, const int level,
               const Vector3<uint64_t>& dims)
    {
        const uint64_t bps = vol->GetBytesPerVoxel();
        const uint64_t size = dims.x * dims.y * dims.z * bps;

        std::cout << "mip " << level << ": size is: "
                  << om::string::humanizeNum(size)
                  << " (" << dims.x
                  << "," << dims.y
                  << "," << dims.z
                  << ")\n";

        const std::string fnpStr = OmFileNames::GetMemMapFileName(vol, level);
        const QString fnp = QString::fromStdString(fnpStr);
        QFile::remove(fnp);
        boost::shared_ptr<QFile> file(om::make_shared<QFile>(fnp));
        file->resize(size);
        if(!file->open(QIODevice::ReadWrite)){
            throw om::IoException("could not open file ",fnpStr.toStdString());
        }
        file->seek(size-1);
        file->putChar(0);
        file->flush();

        return file;
    }
};

