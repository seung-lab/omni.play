#pragma once

#include "common/common.h"
#include "datalayer/dataSource.hpp"
#include "segment/types.hpp"
#include "container/pagedVector.hpp"
#include "container/fileVector.hpp"

namespace om {
namespace segment {

typedef datalayer::Vector<size_t, Data> SegData;
typedef datalayer::Vector<size_t, common::SegListType> SegListData;
typedef datalayer::IDataSource<size_t, SegData> SegDataDS;
typedef datalayer::IDataSource<size_t, SegListData> SegListDataDS;
typedef datalayer::IDataSource<size_t, Edge> EdgeDS;

typedef data::PagedVector<Data> SegDataVector;
typedef data::PagedVector<common::SegListType> SegListDataVector;
typedef data::FileVector<Edge> EdgeVector;
}
}  // namespace om::segment::
