#pragma once

#include "boost/scoped_ptr.hpp"

namespace om {
namespace pipeline {

template <typename Tout>
class out_stage
{
public:
    virtual Tout* operator()() = 0;
    virtual int out_size() = 0;
    virtual void cleanup() = 0;
};

template <typename Tin, typename Tout>
class stage : public out_stage<Tout>
{
public:
    typedef out_stage<Tin> pred_t;

protected:
    pred_t* predecessor_;

public:
    stage(pred_t* predecessor)
        : predecessor_(predecessor)
    {}

    ~stage() {
        cleanup();
    }

    virtual Tout* operator()(Tin*) = 0;
    virtual void cleanup() {};

    Tout* operator()() {
        Tin* data = (*predecessor_)();
        Tout* ret = this->operator()(data);
        predecessor_->cleanup();
        return ret;
    }
};

}
}
