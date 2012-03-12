#pragma once

#include "pipeline/stage.hpp"

#include "boost/scoped_array.hpp"

namespace om {
namespace pipeline {

template <typename T>
class filter : public stage<T, T>
{
protected:
    boost::scoped_array<T> data_;
    int size_;

    virtual bool predicate(const T&) = 0;

public:
    filter(out_stage<T>* predecessor)
        : stage<T, T>(predecessor)
    { }

    T* operator()(T* input)
    {
        std::cout << "Filtering" << std::endl;
        size_ = this->predecessor_->out_size();
        data_.reset(new T[size_]);
        for(int i = 0; i < size_; i++) {
            if(predicate(input[i])) {
                data_[i] = input[i];
            } else {
                data_[i] = 0;
            }
        }
        return data_.get();
    }

    int out_size() const {
        return size_;
    }

    void cleanup() {
        data_.reset();
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
};

} // namespace pipeline
} // namespace om
