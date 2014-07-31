#include "chunk/iterators.hpp"

namespace boost {
template class boost::transform_iterator<
    std::function<om::coords::Chunk(const Vector3i&)>,
    om::utility::vector3_iterator<int>>;
template class boost::filter_iterator<
    std::function<bool(const om::coords::Chunk&)>,
    boost::transform_iterator<std::function<om::coords::Chunk(const Vector3i&)>,
                              om::utility::vector3_iterator<int>>>;
}