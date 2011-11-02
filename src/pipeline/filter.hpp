#pragma once

#include "pipeline/stage.hpp"

namespace om {
namespace pipeline {

template <typename T>
class filter : public stage<T, T>
{
protected:
    scoped_ptr<T> data_;
    int size_;

    virtual bool predicate(const T&) = 0;

public:
    filter(out_stage<T> predecessor)
        : stage<T, T>(predecessor)
    { }

    T* operator()(T* input)
    {
        size_ = predecessor_->out_size();
        data_.reset(new T[size_]);
        for(int i = 0; i < size; i++) {
            if(predicate(input[i])) {
                data_[i] = input[i];
            } else {
                data_[i] = 0;
            }
        }
    }

    int out_size() const {
        return size_;
    }
};

class seg_filter : public filter<uint32_t>
{
private:
    uint32_t segId_;

public:
    seg_filter(out_stage<uint32_t>* predecessor, uint32_t segId)
        : filter<uint32_t>(predecessor)
        , segId_(segId)
    {}

protected:
    bool predicate(const uint32_t& id) {
        return id == segId_;
    }
}

} // namespace pipeline
} // namespace om
