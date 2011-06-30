#pragma once

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

    template <typename VOL>
    void loadOldOmMipVolume(QDataStream& in, VOL& vol)
    {
        if(fileVersion_ < 24)
        {
            QString dead;
            in >> dead;
        }

        in >> vol.Coords().mMipLeafDim;
        in >> vol.Coords().mMipRootLevel;

        if(fileVersion_ < 24)
        {
            qint32 dead;
            in >> dead;
        }

        in >> vol.mBuildState;

        bool dead;
        in >> dead;

        if(fileVersion_ < 24)
        {
            qint32 dead;
            in >> dead;
        }

        if(fileVersion_ > 13)
        {
            QString volDataType;
            in >> volDataType;
            vol.mVolDataType = OmVolumeTypeHelpers::GetTypeFromString(volDataType);

        } else {
            vol.mVolDataType = OmVolDataType::UNKNOWN;
        }

        vol.LoadPath();
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


