#include "segment/page.h"

namespace om {
namespace segment {

page::page(char* data, size_t size)
    : datalayer::cachedType<datalayer::SegmentationData>(data, size)
{ }

const int page::PAGE_SIZE = 100000;

}} // namespace om::segment::
