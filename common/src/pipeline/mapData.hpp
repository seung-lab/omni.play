#pragma once

#include "common/common.h"
#include "pipeline/stage.hpp"
#include "boost/scoped_ptr.hpp"
#include "datalayer/memMappedFile.hpp"

namespace om {
namespace volume {
class Volume;
}
namespace tiles {
class tile;
}
namespace pipeline {

class mapData {
 private:
  dataSrcs file_;

 public:
  mapData(std::string fnp, common::DataType type) {
    switch (type.index()) {
      case common::DataType::INT8:
        init<int8_t>(fnp);
        break;
      case common::DataType::UINT8:
        init<uint8_t>(fnp);
        break;
      case common::DataType::INT32:
        init<int32_t>(fnp);
        break;
      case common::DataType::UINT32:
        init<uint32_t>(fnp);
        break;
      case common::DataType::FLOAT:
        init<float>(fnp);
        break;
    }
  }

  template <typename T> void init(std::string fnp) {
    file_ = datalayer::MemMappedFile<T>(fnp);
  }

  dataSrcs file() const { return file_; }

  operator dataSrcs() { return file_; }
};
}
}
