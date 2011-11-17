#pragma once

#include "common/common.h"
#include "pipeline/stage.hpp"
#include "boost/scoped_ptr.hpp"
#include "datalayer/fs/memMappedFile.hpp"

namespace om {
namespace volume { class volume; }
namespace tiles { class tile; }
namespace pipeline {
/*
template<typename T>
class getTileData : public out_stage<T>
{
private:
    int outSize_;
    volume::volume* vol_;
    coords::chunkCoord coord_;
    common::viewType view_;
    int depth_;
    boost::scoped_ptr<tiles::tile> tile_;

public:
    getTileData(volume::volume* vol,
                coords::chunkCoord coord,
                common::viewType view,
                int depth)
        : vol_(vol)
        , coord_(coord)
        , view_(view)
        , depth_(depth)
    {}

    T* operator()()
    {
        std::cout << "Getting Tile Data." << std::endl;
        tile_.reset(new tiles::tile(vol_, coord_, view_, depth_));
        tile_->loadData();
//        outSize_ = 128 * 128;
        Vector3i dims = vol_->CoordinateSystem().GetChunkDimensions();
        switch(view_)
        {
        case common::XY_VIEW: outSize_ = dims.x * dims.y; break;
        case common::XZ_VIEW: outSize_ = dims.x * dims.z; break;
        case common::ZY_VIEW: outSize_ = dims.y * dims.z; break;
        }
        return tile_->data<T>();
    }

    inline int out_size() const {
        return outSize_;
    }

    void cleanup() {
        tile_.reset();
    }
};
*/

class mapData
{
private:
    dataSrcs file_;

public:
    mapData(std::string fnp, server::dataType::type type)
    {
        switch(type)
        {
        case server::dataType::INT8: init<int8_t>(fnp); break;
        case server::dataType::UINT8: init<uint8_t>(fnp); break;
        case server::dataType::INT32: init<int32_t>(fnp); break;
        case server::dataType::UINT32: init<uint32_t>(fnp); break;
        case server::dataType::FLOAT: init<float>(fnp); break;
        }
    }

    template<typename T>
    void init(std::string fnp) {
        file_ = datalayer::memMappedFile<T>(fnp);;
    }

    operator dataSrcs() {
        return file_;
    }
};

}
}
