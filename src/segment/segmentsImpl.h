#pragma once

#include "segment/lowLevel/segmentsImplLowLevel.h"
#include "datalayer/archive/segmentation.h"

namespace om {
namespace segment {

class segmentsStore;
class segment;

class segmentsImpl : public segmentsImplLowLevel {
public:
    segmentsImpl(segmentation*, segmentsStore*);
    virtual ~segmentsImpl();

    void Flush();

    segment* AddSegment();
    segment* AddSegment(common::segId value);
    segment* GetOrAddSegment(const common::segId val);

//private:
//    friend YAML::Emitter& YAML::operator<<(YAML::Emitter&, const segmentsImpl&);
//    friend void YAML::operator>>(const YAML::Node&, segmentsImpl&);
};

} // namespace segment
} // namespace om
