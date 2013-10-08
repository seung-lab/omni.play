#include "pipeline/chunk.h"

namespace om {
namespace pipeline {

data_var operator>>(const dataSrcs& d, const getChunk& v) {
  return boost::apply_visitor(v, d);
}

}
}