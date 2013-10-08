#pragma once

#include "common/common.h"
#include "pipeline/stage.hpp"
#include "boost/scoped_ptr.hpp"
#include "datalayer/memMappedFile.hpp"

namespace om {
namespace volume {
class volume;
}
namespace tiles {
class tile;
}
namespace pipeline {

class mapData {
 private:
  dataSrcs file_;

 public:
  mapData(std::string fnp, server::dataType::type type) {
    switch (type) {
      case server::dataType::INT8:
        init<int8_t>(fnp);
        break;
      case server::dataType::UINT8:
        init<uint8_t>(fnp);
        break;
      case server::dataType::INT32:
        init<int32_t>(fnp);
        break;
      case server::dataType::UINT32:
        init<uint32_t>(fnp);
        break;
      case server::dataType::FLOAT:
        init<float>(fnp);
        break;
    }
  }

  template <typename T> void init(std::string fnp) {
    file_ = datalayer::memMappedFile<T>(fnp);
    ;
  }

  dataSrcs file() const { return file_; }

  operator dataSrcs() { return file_; }
};

}
}
