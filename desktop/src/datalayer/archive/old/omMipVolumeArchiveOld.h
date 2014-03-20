#pragma once
#include "precomp.h"

#include "volume/omSegmentation.h"
#include "volume/omChannel.h"
#include "volume/omFilter2d.h"

class OmMipVolumeArchiveOld {
 public:
  template <typename VOL>
  static void Load(QDataStream& in, VOL& vol, const int fileVersion) {
    OmMipVolumeArchiveOld old(fileVersion);
    old.Load(in, vol);
  }

  static void LoadOmManageableObject(QDataStream& in, OmManageableObject& mo) {
    in >> mo.id_;
    in >> mo.customName_;
    in >> mo.note_;
  }

  static void StoreOmManageableObject(QDataStream& out,
                                      const OmManageableObject& mo) {
    out << mo.id_;
    out << mo.customName_;
    out << mo.note_;
  }

 private:
  const int fileVersion_;

  OmMipVolumeArchiveOld(const int fileVersion) : fileVersion_(fileVersion) {}

  void Load(QDataStream& in, OmChannel& chan) {
    LoadOmManageableObject(in, chan);
    loadOldOmMipVolume(in, chan);
    loadOldOmVolume(in, chan.Coords());
  }

  void Load(QDataStream& in, OmSegmentation& seg) {
    LoadOmManageableObject(in, seg);
    loadOldOmVolume(in, seg.Coords());
    loadOldOmMipVolume(in, seg);
  }

  template <typename VOL>
  void loadOldOmMipVolume(QDataStream& in, VOL& vol) {
    if (fileVersion_ < 24) {
      QString dead;
      in >> dead;
    }

    int mMipLeafDim;
    in >> mMipLeafDim;
    int mMipRootLevel;
    in >> mMipRootLevel;

    if (fileVersion_ < 24) {
      qint32 dead;
      in >> dead;
    }

    in >> vol.mBuildState;

    bool dead;
    in >> dead;

    if (fileVersion_ < 24) {
      qint32 dead;
      in >> dead;
    }

    if (fileVersion_ > 13) {
      QString volDataType;
      in >> volDataType;
      vol.mVolDataType = OmVolumeTypeHelpers::GetTypeFromString(volDataType);

    } else {
      vol.mVolDataType = OmVolDataType::UNKNOWN;
    }

    vol.LoadPath();
  }

  static void loadOldOmVolume(QDataStream& in, om::coords::VolumeSystem& v) {
    Matrix4f dummyMat;
    in >> dummyMat;  // normToDataMat_s
    in >> dummyMat;  // normToDataInvMat_
    AxisAlignedBoundingBox<int> extent;
    in >> extent;
    v.SetDataDimensions(extent.getDimensions());
    Vector3f resolution;
    in >> resolution;
    v.SetResolution(resolution);
    int chunkDim;
    in >> chunkDim;
    v.SetChunkDimensions(Vector3i(chunkDim));
    QString dummy;
    in >> dummy;  // c.unitString_
    Vector3f dummyVec;
    in >> dummyVec;  // c.dataStretchValues_
  }
};
