#include "volume/iterators.hpp"

namespace om {
namespace volume {
template class dataval_iterator<int8_t, chunk::iterator>;
template class dataval_iterator<uint8_t, chunk::iterator>;
template class dataval_iterator<int32_t, chunk::iterator>;
template class dataval_iterator<uint32_t, chunk::iterator>;
template class dataval_iterator<float, chunk::iterator>;

template class dataval_iterator<int8_t, chunk::filtered_iterator>;
template class dataval_iterator<uint8_t, chunk::filtered_iterator>;
template class dataval_iterator<int32_t, chunk::filtered_iterator>;
template class dataval_iterator<uint32_t, chunk::filtered_iterator>;
template class dataval_iterator<float, chunk::filtered_iterator>;
}
}

namespace boost {
using namespace om::volume;
// BOOST static assert failing for some reason.
// template class boost::filter_iterator<
//     std::function<bool(const CoordValue<int8_t>&)>,
//     chunk_filtered_dataval_iterator<int8_t>>;
// template class boost::filter_iterator<
//     std::function<bool(const CoordValue<uint8_t>&)>,
//     chunk_filtered_dataval_iterator<uint8_t>>;
// template class boost::filter_iterator<
//     std::function<bool(const CoordValue<int32_t>&)>,
//     chunk_filtered_dataval_iterator<int32_t>>;
// template class boost::filter_iterator<
//     std::function<bool(const CoordValue<uint32_t>&)>,
//     chunk_filtered_dataval_iterator<uint32_t>>;
// template class boost::filter_iterator<
//     std::function<bool(const CoordValue<float>&)>,
//     chunk_filtered_dataval_iterator<float>>;

// template class boost::filter_iterator<
//     std::function<bool(const CoordValue<int8_t>&)>,
//     all_dataval_iterator<int8_t>>;
// template class boost::filter_iterator<
//     std::function<bool(const CoordValue<uint8_t>&)>,
//     all_dataval_iterator<uint8_t>>;
// template class boost::filter_iterator<
//     std::function<bool(const CoordValue<int32_t>&)>,
//     all_dataval_iterator<int32_t>>;
// template class boost::filter_iterator<
//     std::function<bool(const CoordValue<uint32_t>&)>,
//     all_dataval_iterator<uint32_t>>;
// template class boost::filter_iterator<
//     std::function<bool(const CoordValue<float>&)>,
// all_dataval_iterator<float>>;
}