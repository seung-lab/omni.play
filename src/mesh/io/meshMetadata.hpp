#pragma once

#include "volume/segmentation.h"
#include "volume/segmentationFolder.h"
#include "datalayer/fs/omFileNames.hpp"

class meshMetadata {
private:
    segmentation *const segmentation_;
    const double threshold_;
    const QString fnp_;

    int fileVersion_;

    bool meshBuilt_;

    // 1 == hdf5
    // 2 == per-chunk files
    int meshVersion_;

    zi::spinlock lock_;

public:
    meshMetadata(segmentation *segmentation, const double threshold)
        : segmentation_(segmentation)
        , threshold_(threshold)
        , fnp_(filePath())
        , fileVersion_(1)
        , meshBuilt_(false)
        , meshVersion_(0)
    {}

    bool Load()
    {
        zi::guard g(lock_);
        return load();
    }

    bool IsBuilt() const
    {
        zi::guard g(lock_);
        return meshBuilt_;
    }

    bool IsHDF5() const
    {
        zi::guard g(lock_);
        if(!meshVersion_){
            throw OmIoException("meshes not yet built");
        }
        return 1 == meshVersion_;
    }

    void SetMeshedAndStorageAsHDF5()
    {
        zi::guard g(lock_);
        meshBuilt_ = true;
        meshVersion_ = 1;
        store();
    }

    void SetMeshedAndStorageAsChunkFiles()
    {
        zi::guard g(lock_);
        meshBuilt_ = true;
        meshVersion_ = 2;
        store();
    }

private:
    QString filePath(){
        return segmentation_->Folder()->MeshMetadataFilePerThreshold(threshold_);
    }

    bool load()
    {
        QFile file(fnp_);

        if(!file.exists()){
            return false;
        }

        if(!file.open(QIODevice::ReadOnly)){
            throw OmIoException("error reading file", fnp_);
        }

        QDataStream in(&file);
        in.setByteOrder( QDataStream::LittleEndian );
        in.setVersion(QDataStream::Qt_4_6);

        in >> fileVersion_;
        in >> meshBuilt_;
        in >> meshVersion_;

        if(!in.atEnd()){
            throw OmIoException("corrupt file?", fnp_);
        }

        return true;
    }

    void store()
    {
        QFile file(fnp_);

        if (!file.open(QIODevice::WriteOnly)) {
            throw OmIoException("could not write file", fnp_);
        }

        QDataStream out(&file);
        out.setByteOrder( QDataStream::LittleEndian );
        out.setVersion(QDataStream::Qt_4_6);

        out << fileVersion_;
        out << meshBuilt_;
        out << meshVersion_;
    }
};

