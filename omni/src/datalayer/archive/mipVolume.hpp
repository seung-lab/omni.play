#pragma once

#include "yaml-cpp/yaml.h"
#include "volume/omVolumeTypes.hpp"
#include "datalayer/fs/omFileNames.hpp"
#include "volume/omChannelFolder.h"
#include "volume/omSegmentationFolder.h"

#include <QString>
#include <QFile>

namespace om {
namespace data {
namespace archive {
    
template <typename VOL>
class mipVolume {
private:
    VOL& vol_;
    
public:
    mipVolume(VOL& vol)
    : vol_(vol)
    {}
    
    void Store(YAML::Emitter& out) const
    {
        out << YAML::Key << "id" << YAML::Value << vol_.id_;
        out << YAML::Key << "custom name" << YAML::Value << vol_.customName_;
        out << YAML::Key << "note" << YAML::Value << vol_.note_;
        
        out << YAML::Key << "coords" << YAML::Value << vol_.Coords();
        
        out << YAML::Key << "build state" << YAML::Value << vol_.mBuildState;
        
        const std::string type =
        OmVolumeTypeHelpers::GetTypeAsString(vol_.mVolDataType);
        out << YAML::Key << "type" << YAML::Value << type;
        
        save();
    }
    
    void Load(const YAML::Node& in)
    {
        in["id"] >> vol_.id_;
        in["custom name"] >> vol_.customName_;
        in["note"] >> vol_.note_;
        
        in["coords"] >> vol_.Coords();
        
        in["build state"] >> vol_.mBuildState;
        
        QString volDataType;
        in["type"] >> volDataType;
        vol_.mVolDataType = OmVolumeTypeHelpers::GetTypeFromString(volDataType);
        
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
            throw OmIoException("error reading file", filePath);
        }
        
        QDataStream in(&file);
        in.setByteOrder(QDataStream::LittleEndian);
        in.setVersion(QDataStream::Qt_4_6);
        
        int version;
        in >> version;
        
        assert(1 == version);
        
        Vector3f vec;
        in >> vec;
        vol_.Coords().SetAbsOffset(vec);
        
        if(!in.atEnd()){
            throw OmIoException("corrupt file?", filePath);
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
        
        out << vol_.Coords().GetAbsOffset();
    }
};

}; // namespace archive
}; // namespace data
}; // namespace om