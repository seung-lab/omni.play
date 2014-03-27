#pragma once
#include "precomp.h"

#include "datalayer/archive/old/utilsOld.hpp"
#include "datalayer/fs/omFile.hpp"
#include "utility/yaml/coords.h"

#include "volume/omMipVolume.h"
#include "volume/omVolumeTypes.hpp"

template <typename VOL>
class OmMipVolumeArchive {
 private:
  VOL& vol_;

 public:
  OmMipVolumeArchive(VOL& vol) : vol_(vol) {}

  void Store(QDataStream& out) const {
    out << vol_.id_;
    out << vol_.customName_;
    out << vol_.note_;

    out << vol_.Coords();

    out << vol_.mBuildState;

    const QString type =
        OmVolumeTypeHelpers::GetTypeAsQString(vol_.mVolDataType);
    out << type;
    log_infos << "saved type as " << type.toStdString();

    save();
  }

  void Load(QDataStream& in) {
    in >> vol_.id_;
    in >> vol_.customName_;
    in >> vol_.note_;

    in >> vol_.Coords();

    in >> vol_.mBuildState;

    QString volDataType;
    in >> volDataType;
    vol_.mVolDataType = OmVolumeTypeHelpers::GetTypeFromString(volDataType);

    vol_.LoadPath();

    load();
  }

 private:
  QString filePathV1() const { return vol_.VolPaths().AbsCoord(0).c_str(); }

  void load() {
    const QString filePath = filePathV1();

    QFile file(filePath);

    if (!file.exists()) {
      return;
    }

    if (!file.open(QIODevice::ReadOnly)) {
      throw om::IoException("error reading file");
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

    if (!in.atEnd()) {
      throw om::IoException("corrupt file?");
    }
  }

  void save() const {
    const QString filePath = filePathV1();

    QFile file(filePath);

    om::file::old::openFileWO(file);

    QDataStream out(&file);
    out.setByteOrder(QDataStream::LittleEndian);
    out.setVersion(QDataStream::Qt_4_6);

    const int version = 1;
    out << version;

    out << vol_.Coords().GetAbsOffset();
  }
};

QDataStream& operator<<(QDataStream& out, const om::coords::VolumeSystem& c);
QDataStream& operator>>(QDataStream& in, om::coords::VolumeSystem& c);
