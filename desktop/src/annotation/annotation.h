#pragma once

#include "common/omCommon.h"
#include "datalayer/fs/omFile.hpp"
#include "system/omGenericManager.hpp"
#include "events/omEvents.h"

#include <QDataStream>
#include <QFile>
#include <QVector>

class OmSegmentation;

namespace om {
namespace annotation {

struct data {
    OmID id;
    globalCoord coord;
    std::string comment;
    OmColor color;
    
    data(OmID id) : id(id) {}
    
    inline OmID GetID() { return id; }
};

class manager : public OmGenericManager<data> {
private:
    OmSegmentation *const vol_;
    typedef OmGenericManager<data> base_t;
    
public:
    manager(OmSegmentation* vol)
        : vol_(vol)
    {}

    void Add(globalCoord coord, const std::string& comment, const OmColor& color)
    {
        data& d = base_t::Add();

        d.coord = coord;
        d.comment = comment;
        d.color = color;
        OmEvents::AnnotationEvent();
        OmEvents::Redraw2d();
        OmEvents::Redraw3d();
    }
    
    void Load();
    void Save() const;
    

protected:
    std::string getFileName() { return filePathV1(); }

private:
    std::string filePathV1() const;
};

} // namespace annotation
} // namespace om
