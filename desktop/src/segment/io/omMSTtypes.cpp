#include "segment/io/omMSTtypes.h"

std::ostream& operator<<(std::ostream& out, const OmMSTImportEdge& e) {
  out << "[ " << e.node1ID << ", " << e.node2ID << ", " << e.threshold << "]";

  return out;
}
