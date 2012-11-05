#pragma once

#include <boost/scoped_ptr.hpp>
#include <boost/variant.hpp>

#include "common/common.h"
#include "datalayer/types.hpp"

namespace om {
namespace chunks {

template<typename T>
class Chunk
{
private:
    Chunk<T>* data_;

public:
    Chunk(char* data, size_t size)
    {
        if(size != sizeof(Chunk<T>)) {
            throw ArgException("Chunk Data is the wrong size");
            data_ = reinterpret_cast<Chunk<T> >(data);
        }
    }

    const T& operator[](uint i) const
    {
        if(i >= 128*128*128) {
            throw ArgException("chunk index out of bounds.");
        }

        return data_->data[i];
    }

    T& operator[](uint i) {
        if(i >= 128*128*128) {
            throw ArgException("chunk index out of bounds.");
        }

        return data_->data[i];
    }
};

typedef boost::variant<Chunk<int8_t>,
                       Chunk<uint8_t>,
                       Chunk<int32_t>,
                       Chunk<uint32_t>,
                       Chunk<float> > ChunkVar;

}} // namespace om::chunks::
