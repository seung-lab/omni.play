#pragma once

#include "yaml-cpp/yaml.h"
#include "volume/omVolumeTypes.hpp"
#include "datalayer/fs/omFileNames.hpp"
#include "volume/omChannelFolder.h"
#include "volume/omSegmentationFolder.h"

#include <QString>
#include <QFile>

namespace YAML {
    
template <typename VOL>
class mipVolume {
private:
    VOL& vol_;
    
public:
    mipVolume(VOL& vol)
    : vol_(vol)
    {}
    
    void Store(Emitter& out) const
    {
        out << Key << "id" << Value << vol_.id_;
        out << Key << "custom name" << Value << vol_.customName_;
        out << Key << "note" << Value << vol_.note_;
        
        out << Key << "coords" << Value << vol_.Coords();
        
        out << Key << "build state" << Value << vol_.mBuildState;
        
        const std::string type =
        OmVolumeTypeHelpers::GetTypeAsString(vol_.mVolDataType);
        out << Key << "type" << Value << type;
        
        save();
    }
    
    void Load(const Node& in)
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

} // namespace YAML