#ifndef OM_MIP_VOLUME_ARCHIVE_OLD_H
#define OM_MIP_VOLUME_ARCHIVE_OLD_H

#include "volume/omSegmentation.h"
#include "volume/omChannel.h"

#include <QDataStream>

class OmMipVolumeArchiveOld {
public:
    template <typename VOL>
    static void Load(QDataStream& in, VOL& vol, const int fileVersion)
    {
        OmMipVolumeArchiveOld old(fileVersion);
        old.Load(in, vol);
    }

    static void LoadOmManageableObject(QDataStream& in, OmManageableObject& mo)
    {
        in >> mo.id_;
        in >> mo.customName_;
        in >> mo.note_;
    }

    static void StoreOmManageableObject(QDataStream& out,
                                        const OmManageableObject& mo)
    {
        out << mo.id_;
        out << mo.customName_;
        out << mo.note_;
    }

private:
    const int fileVersion_;

    OmMipVolumeArchiveOld(const int fileVersion)
        : fileVersion_(fileVersion)
    {}

    void Load(QDataStream& in, OmChannel& chan)
    {
        LoadOmManageableObject(in, chan);
        loadOldOmMipVolume(in, chan);
        loadOldOmVolume(in, chan.Coords());
    }

    void Load(QDataStream& in, OmSegmentation& seg)
    {
        LoadOmManageableObject(in, seg);
        loadOldOmVolume(in, seg.Coords());
        loadOldOmMipVolume(in, seg);
    }

    void loadOldOmMipVolume(QDataStream& in, OmMipVolume& m)
    {
        if(fileVersion_ < 24){
            QString dead;
            in >> dead;
        }

        in >> m.Coords().mMipLeafDim;
        in >> m.Coords().mMipRootLevel;

        if(fileVersion_ < 24){
            qint32 dead;
            in >> dead;
        }

        in >> m.mBuildState;

        bool dead;
        in >> dead;

        if(fileVersion_ < 24){
            qint32 dead;
            in >> dead;
        }

        if(fileVersion_ > 13){
            QString volDataType;
            in >> volDataType;
            m.mVolDataType = OmVolumeTypeHelpers::GetTypeFromString(volDataType);
        } else {
            m.mVolDataType = OmVolDataType::UNKNOWN;
        }
    }

    static void loadOldOmVolume(QDataStream& in, OmMipVolCoords& v)
    {
        in >> v.normToDataMat_;
        in >> v.normToDataInvMat_;
        in >> v.dataExtent_;
        in >> v.dataResolution_;
        in >> v.chunkDim_;
        in >> v.unitString_;
        in >> v.dataStretchValues_;
    }
};


#endif
