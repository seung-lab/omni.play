#pragma once
#include "precomp.h"

#include "volume/omSegmentation.h"

class OmMeshMetadata {
 private:
  OmSegmentation *const segmentation_;
  const double threshold_;
  const QString fnp_;

  int fileVersion_;

  bool meshBuilt_;

  // 1 == hdf5
  // 2 == per-chunk files
  int meshVersion_;

  zi::spinlock lock_;

 public:
  OmMeshMetadata(OmSegmentation *segmentation, const double threshold)
      : segmentation_(segmentation),
        threshold_(threshold),
        fnp_(filePath()),
        fileVersion_(1),
        meshBuilt_(false),
        meshVersion_(0) {}

  bool Load() {
    zi::guard g(lock_);
    return load();
  }

  bool IsBuilt() const {
    zi::guard g(lock_);
    return meshBuilt_;
  }

  bool IsHDF5() const {
    zi::guard g(lock_);
    if (!meshVersion_) {
      throw om::IoException("meshes not yet built");
    }
    return 1 == meshVersion_;
  }

  void SetMeshedAndStorageAsHDF5() {
    zi::guard g(lock_);
    meshBuilt_ = true;
    meshVersion_ = 1;
    store();
  }

  void SetMeshedAndStorageAsChunkFiles() {
    zi::guard g(lock_);
    meshBuilt_ = true;
    meshVersion_ = 2;
    store();
  }

 private:
  QString filePath() {
    return segmentation_->SegPaths().MeshMetaData().c_str();
  }

  bool load() {
    QFile file(fnp_);

    if (!file.exists()) {
      return false;
    }

    if (!file.open(QIODevice::ReadOnly)) {
      throw om::IoException("error reading file");
    }

    QDataStream in(&file);
    in.setByteOrder(QDataStream::LittleEndian);
    in.setVersion(QDataStream::Qt_4_6);

    in >> fileVersion_;
    in >> meshBuilt_;
    in >> meshVersion_;

    if (!in.atEnd()) {
      throw om::IoException("corrupt file?");
    }

    return true;
  }

  void store() {
    QFile file(fnp_);

    if (!file.open(QIODevice::WriteOnly)) {
      throw om::IoException("could not write file");
    }

    QDataStream out(&file);
    out.setByteOrder(QDataStream::LittleEndian);
    out.setVersion(QDataStream::Qt_4_6);

    out << fileVersion_;
    out << meshBuilt_;
    out << meshVersion_;
  }
};
