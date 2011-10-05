#pragma once

#include "volume/segmentationFolder.h"
#include "datalayer/archive/old/dataArchiveStd.hpp"
#include "datalayer/fs/omFileNames.hpp"
#include "utility/omLockedPODs.hpp"
#include "segment/segments.h"
#include "utility/dataWrappers.h"

#include <QVector> //TODO: switch to mem-mapped file...

class OmValidGroupNum {
private:
    segmentation* vol_;
    int version_;

    const uint32_t noGroupNum_;
    const uint32_t initialGroupNum_;

    LockedUint32 maxGroupNum_;
    std::vector<uint32_t> segToGroupNum_;

public:
    OmValidGroupNum(segmentation* segmentation)
        : vol_(segmentation)
        , version_(1)
        , noGroupNum_(0)
        , initialGroupNum_(1)
    {
        maxGroupNum_.set(initialGroupNum_);
    }

    void Load() {
        load();
    }

    void Save() const {
        save();
    }

    void Resize(const size_t size){
        segToGroupNum_.resize(size, noGroupNum_);
    }

    void Clear()
    {
        std::fill(segToGroupNum_.begin(), segToGroupNum_.end(), noGroupNum_);
        maxGroupNum_.set(initialGroupNum_);
    }

    template <typename C>
    void Set(const C& segs, const bool isValid)
    {
        const uint32_t groupNum = isValid ? maxGroupNum_.inc() : noGroupNum_;

        FOR_EACH(iter, segs)
        {
            segment* seg = *iter;
            const segId segID = seg->value();
            segToGroupNum_[segID] = groupNum;
        }
    }

    inline bool InSameValidGroup(const segId segID1, const segId segID2) const {
        return segToGroupNum_[segID1] == segToGroupNum_[segID2];
    }

    inline uint32_t Get(const segId segID) const {
          return segToGroupNum_[segID];
    }

    inline uint32_t Get(segment* seg) const {
        return Get(seg->value());
    }

private:
    QString filePathV1() const
    {
        return QString::fromStdString(
            vol_->Folder()->GetVolSegmentsPathAbs("valid_group_num.data.ver1")
            );
    }

    void load()
    {
        const QString filePath = filePathV1();

        QFile file(filePath);

        if(!file.exists()){
            return;
        }

        om::file::openFileRO(file);

        QDataStream in(&file);
        in.setByteOrder(QDataStream::LittleEndian);
        in.setVersion(QDataStream::Qt_4_6);

        in >> version_;
        in >> segToGroupNum_;

        quint64 maxGroupNum;
        in >> maxGroupNum;
        maxGroupNum_.set(maxGroupNum);

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

        out << version_;
        out << segToGroupNum_;

        const quint64 maxGroupNum = maxGroupNum_.get();
        out << maxGroupNum;
    }
};

