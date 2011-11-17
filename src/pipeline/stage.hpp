#pragma once

#include "boost/shared_ptr.hpp"
#include "boost/variant.hpp"
#include "volume/volumeTypes.h"
#include "datalayer/fs/memMappedFile.hpp"

namespace om {
namespace pipeline {

template<typename T>
struct data
{
    boost::shared_ptr<T> data;
    uint64_t size;
};

typedef boost::variant<datalayer::memMappedFile<int8_t>,
                       datalayer::memMappedFile<uint8_t>,
                       datalayer::memMappedFile<int32_t>,
                       datalayer::memMappedFile<uint32_t>,
                       datalayer::memMappedFile<float> > dataSrcs;

typedef boost::variant<data<bool>,
                       data<char>,
                       data<int8_t>,
                       data<uint8_t>,
                       data<int32_t>,
                       data<uint32_t>,
                       data<float> > data_var;

typedef boost::static_visitor<data_var> stage;

template<typename T>
data_var operator>>(const data_var& d, const T& v) {
    return boost::apply_visitor(v, d);
}

/*
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
*/
}
}
