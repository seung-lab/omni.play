#pragma once

#include "pipeline/stage.hpp"
#include "b64/encode.h"

#include "boost/scoped_ptr.hpp"

namespace om {
namespace pipeline {

class encode : public stage<char, char>
{
private:
    boost::scoped_ptr<char> data_;
    int outSize_;
    base64::encoder e;

public:
    encode(out_stage<char>* pred)
        : stage<char, char>(pred)
    { }

    ~encode() {}

    inline char* operator()(char* input)
    {
        std::cout << "Encoding" << std::endl;
        outSize_ = 2 * predecessor_->out_size();
        data_.reset(new char[outSize_]);
        e.encode(input, predecessor_->out_size(), data_.get());
        return data_.get();
    }

    inline char* operator()(){
        return stage<char, char>::operator()();
    }

    inline void cleanup() {
//        data_.reset();
    }

    inline int out_size() const {
        return outSize_;
    }
};

}
}
