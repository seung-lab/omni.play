#pragma once

#include "pipeline/stage.hpp"

#include "boost/scoped_array.hpp"

namespace om {
namespace pipeline {

template<typename T>
class bitmask : public stage<T, bool>
{
private:
    T mask_;
    boost::scoped_array<bool> data_;
    int outSize_;

public:
    bitmask(out_stage<T>* pred, T mask)
        : stage<T, bool>(pred)
        , mask_(mask)
    {}

    int out_size() const {
        return outSize_;
    }

    void cleanup() {
        this->predecessor_->cleanup();
    }

    bool* operator()(T* data)
    {
        std::cout << "Bitmasking" << std::endl;
        outSize_ = this->predecessor_->out_size();
        data_.reset(new bool[outSize_]);
        for(int i = 0; i < outSize_; i++) {
            data_[i] = data[i] == mask_;
        }
        return data_.get();
    }
};


}
}
