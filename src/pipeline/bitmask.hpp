#pragma once

#include "pipeline/stage.hpp"

#include "boost/scoped_array.hpp"

namespace om {
namespace pipeline {

class bitmask : public stage
{
private:
    uint32_t mask_;

public:
    bitmask(uint32_t mask)
        : mask_(mask)
    {}

    template<typename T>
    data_var operator()(const data<T>& in) const
    {
        std::cout << "Bitmasking" << std::endl;
        data<bool> out;
        out.size = in.size;
        out.data.reset(new bool[out.size]);
        for(int i = 0; i < out.size; i++) {
            out.data.get()[i] = in.data.get()[i] == mask_;
        }
        return out;
    }
};


}
}
