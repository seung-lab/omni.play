#pragma once

#include "boost/scoped_ptr.hpp"

namespace om {
namespace pipeline {

template <typename Tout>
class out_stage
{
public:
    virtual Tout* operator()() = 0;
    virtual int out_size() const = 0;
    virtual void cleanup() = 0;
};

template <typename Tin, typename Tout>
class stage : public out_stage<Tout>
{
protected:
    out_stage<Tin>* predecessor_;
    int here_;

public:
    stage(out_stage<Tin>* predecessor)
        : predecessor_(predecessor)
    {}

    ~stage() {
        cleanup();
    }

    virtual Tout* operator()(Tin*) = 0;
    virtual void cleanup() const {};

    Tout* operator()() {
        Tin* data = (*predecessor_)();
        Tout* ret = this->operator()(data);
        predecessor_->cleanup();
        return ret;
    }
};

}
}
