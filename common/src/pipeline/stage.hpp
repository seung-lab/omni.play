#pragma once

#include "boost/variant.hpp"
#include "datalayer/memMappedFile.hpp"
#include "utility/malloc.hpp"

namespace om {
namespace pipeline {

template <typename T>
struct Data {
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

template <typename T>
data_var operator>>(const data_var& d, const T& v) {
  return boost::apply_visitor(v, d);
}
}
}
