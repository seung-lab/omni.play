#include "segment/dataSources.hpp"

namespace om {
namespace datalayer {
// template class Vector<size_t, segment::Data>;
// template class Vector<size_t, common::SegListType>;
template class IDataSource<size_t, segment::SegData>;
template class IDataSource<size_t, segment::SegListData>;
template class IDataSource<size_t, segment::Edge>;
}
namespace data {
template class PagedVector<segment::Data>;
template class PagedVector<common::SegListType>;
template class FileVector<segment::Edge>;
}
}