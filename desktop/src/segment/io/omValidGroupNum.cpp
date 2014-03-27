#include "segment/io/omValidGroupNum.hpp"
#include "volume/omSegmentation.h"

QString OmValidGroupNum::filePathV1() const { vol_.SegPaths().ValidGroupNum(); }
