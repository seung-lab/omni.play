#pragma once

#include "datalayer/fs/omFileNames.hpp"
#include "volume/omChannelFolder.h"
#include "volume/omMipVolume.h"
#include "volume/omVolumeTypes.hpp"
#include "utility/serializers.hpp"

#include <QDataStream>

template <typename VOL>
class OmMipVolumeArchive {
private:
    VOL& vol_;

public:
    OmMipVolumeArchive(VOL& vol)
        : vol_(vol)
    {}

    // void Store(QDataStream& out) const
    // {
    //     out << vol_.id_;
    //     out << vol_.customName_;
    //     out << vol_.note_;

    //     out << vol_.Coords();

    //     out << vol_.mBuildState;

    //     const std::string type =
    //         OmVolumeTypeHelpers::GetTypeAsString(vol_.mVolDataType);
    //     out << QString::fromStdString(type);
    //     std::cout << "saved type as " << type << "\n";

    //     save();
    // }

    void Load(QDataStream& in)
    {
        in >> vol_.id_;
        in >> vol_.customName_;
        in >> vol_.note_;

        in >> vol_.Coords();

        in >> vol_.mBuildState;

        QString volDataType;
        in >> volDataType;
        vol_.mVolDataType = OmVolumeTypeHelpers::GetTypeFromString(volDataType.toStdString());

        vol_.LoadPath();

        load();
    }

private:
    QString filePathV1() const {
        return vol_.Folder()->GetVolPath() + "abs_coord.ver1";
    }

    void load()
    {
        const QString filePath = filePathV1();

        QFile file(filePath);

        if(!file.exists()){
            return;
        }

        if(!file.open(QIODevice::ReadOnly)){
            throw om::IoException("error reading file", filePath.toStdString());
        }

        QDataStream in(&file);
        in.setByteOrder(QDataStream::LittleEndian);
        in.setVersion(QDataStream::Qt_4_6);

        int version;
        in >> version;

        assert(1 == version);

        Vector3i offset;
        in >> offset;
        vol_.Coords().SetAbsOffset(offset);

        if(!in.atEnd()){
            throw om::IoException("corrupt file?", filePath.toStdString());
        }
    }

    void save() const
    {
        const QString filePath = filePathV1();

        QFile file(filePath);

        om::file::openFileWO(file);

        QDataStream out(&file);
        out.setByteOrder(QDataStream::LittleEndian);
        out.setVersion(QDataStream::Qt_4_6);

        const int version = 1;
        out << version;

        out << vol_.Coords().AbsOffset();
    }
};

QDataStream& operator>>(QDataStream& in, om::coords::VolumeSystem& c);

