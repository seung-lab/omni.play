#pragma once

#include "pipeline/stage.hpp"
#include <fstream>

namespace om {
namespace pipeline {

template<typename T>
class passthrough : public stage<T, T>
{
protected:
    virtual void inspect(const T* data) = 0;

public:
    passthrough(pred_t* pred)
        : stage<T,T>(pred)
    {}

    T* operator()(T* data)
    {
        outSize_ = predecessor_->out_size();
        inspect(data);
        return data;
    }

    void cleanup() {
        predecessor_.cleanup();
    }
};

namespace<T>
class write_out : public passthrough<T>
{
private:
    std::string fnp_;

public:
    write_out(pred_t pred, std::string fnp)
        : passthrough(pred)
        , fnp_(fnp)
    {}

protected:
    void inspect(const T* data)
    {
        std::ofstream fout(fnp_);

        for(int i = 0; i < outSize_; i++) {
            fout << data[i];
        }
    }
};


} // namespace pipeline
} // namespace om
