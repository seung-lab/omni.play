#pragma once

#include "pipeline/stage.hpp"
#include "b64/encode.h"

namespace om {
namespace pipeline {

class encode : public stage<char, char>
{
private:
    char* data_;
    int outSize_;
    base64::encoder e;

public:
    encode(out_stage<char>* pred)
        : stage<char, char>(pred)
    { }

    inline char* operator()(char* input)
    {
        outSize_ = 4 * (predecessor_->out_size() / 3 + 1);
        data_ = new char[outSize_];
        e.encode(input, predecessor_->out_size(), data_);
        return data_;
    }

    inline char* operator()(){
        return stage<char, char>::operator()();
    }

    inline void cleanup() {
        delete data_;
    }

    inline int out_size() {
        return outSize_;
    }
};

}
}
