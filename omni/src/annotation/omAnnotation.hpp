#pragma once

#include "common/omCommon.h"
#include "common/omColors.h"
#include "datalayer/fs/omFile.hpp"
#include "zi/omMutex.h"

#include <QDataStream>
#include <QFile>
#include <QVector>

class OmSegmentation;

namespace om {
namespace annotation {

struct data {
    uint32_t id;
    uint32_t x;
    uint32_t y;
    uint32_t z;
    std::string comment;
    OmColor color;
};

class manager {
private:
    OmSegmentation *const vol_;

    std::vector<data> annotations_;
    zi::spinlock annotationsLock_;

public:
    manager(OmSegmentation* vol)
        : vol_(vol)
    {}

    uint32_t Add(const uint32_t x,
                 const uint32_t y,
                 const uint32_t z,
                 const std::string& comment,
                 const OmColor& color)
    {
        zi::guard g(annotationsLock_);

        const uint32_t id = annotations_.size();
        annotations_.push_back(data());

        data& d = annotations_[id];

        d.id = id;
        d.x = x;
        d.y = y;
        d.z = z;
        d.comment = comment;
        d.color = color;

        return id;
    }

    void Load();
    void Save() const;

private:
    QString filePathV1() const;
};

} // namespace annotation
} // namespace om
