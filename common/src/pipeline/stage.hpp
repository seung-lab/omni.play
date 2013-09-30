#pragma once

#include "boost/variant.hpp"
#include "datalayer/memMappedFile.hpp"
#include "utility/malloc.hpp"

namespace om {
namespace pipeline {

template <typename T> struct Data {
  size_t size;
  std::shared_ptr<T> data;

  Data() {}
  Data(std::shared_ptr<T> data, size_t size) : size(size), data(data) {}
  Data(size_t size)
      : size(size),
        data(mem::Malloc<T>::NumElements(size, mem::ZeroFill::DONT)) {}
};

typedef boost::variant<
    datalayer::MemMappedFile<int8_t>, datalayer::MemMappedFile<uint8_t>,
    datalayer::MemMappedFile<int32_t>, datalayer::MemMappedFile<uint32_t>,
    datalayer::MemMappedFile<float>> dataSrcs;

typedef boost::variant<Data<bool>, Data<char>, Data<int8_t>, Data<uint8_t>,
                       Data<int32_t>, Data<uint32_t>, Data<float>> data_var;

typedef boost::static_visitor<data_var> stage;

template <typename T> data_var operator>>(const data_var& d, const T& v) {
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
