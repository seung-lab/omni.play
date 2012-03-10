#pragma once

#include "datalayer/fs/omFile.hpp"
#include "datalayer/fs/omFileNames.hpp"
#include "common/omCommon.h"

class OmSegmentPageVersion {
private:
    // version 1: pages in hdf5
    // version 2: first move to mem-mapped pages
    // version 3: replace 'bool immutable' w/ 'enum OmSegListType'
    // version 4: split om::SegListType off into seperate file
    static const int CurrentFileVersion = 4;

    OmSegmentation *const vol_;
    const PageNum pageNum_;
    const QString fnp_;

    int version_;

public:
    OmSegmentPageVersion(OmSegmentation* vol, const PageNum pageNum)
        : vol_(vol)
        , pageNum_(pageNum)
        , fnp_(versionFilePath())
        , version_(0)
    {}

    int Get() const {
        return version_;
    }

    void Load()
    {
        if(!QFile::exists(fnp_))
        {
            version_ = 1;
            return;
        }

        loadVersion();
    }

    void SetAsLatest()
    {
        version_ = CurrentFileVersion;
        storeVersion();
    }

private:

    void storeVersion()
    {
        QFile file(fnp_);

        om::file::openFileRW(file);

        QDataStream out(&file);
        out.setByteOrder( QDataStream::LittleEndian );
        out.setVersion(QDataStream::Qt_4_6);

        out << version_;
    }

    void loadVersion()
    {
        QFile file(fnp_);

        om::file::openFileRO(file);

        QDataStream in(&file);
        in.setByteOrder( QDataStream::LittleEndian );
        in.setVersion(QDataStream::Qt_4_6);

        in >> version_;

        if(!in.atEnd()){
            throw OmIoException("corrupt file?", versionFilePath());
        }
    }

    QString versionFilePath(){
        return memMapPathQStrV2() + ".ver";
    }

    QString memMapPathQStrV2()
    {
        const std::string fname = str( boost::format("segment_page%1%.data")
                                       % pageNum_);

        return QString::fromStdString(
            vol_->Folder()->GetVolSegmentsPathAbs(fname)
            );
    }
};

