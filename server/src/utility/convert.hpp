#pragma once

#include "common/common.h"
#include "common/enums.hpp"
#include "server_types.h"

namespace om {
namespace utility {

inline server::vector3d Convert(coords::Global vec)
{
	server::vector3d ret;
	ret.x = vec.x; ret.y = vec.y; ret.z = vec.z;
	return ret;
}

inline coords::Global Convert(server::vector3d vec)
{
	coords::Global ret;
	ret.x = vec.x; ret.y = vec.y; ret.z = vec.z;
	return ret;
}

inline server::vector3i Convert(Vector3i vec)
{
	server::vector3i ret;
	ret.x = vec.x; ret.y = vec.y; ret.z = vec.z;
	return ret;
}

inline Vector3i Convert(server::vector3i vec)
{
	Vector3i ret;
	ret.x = vec.x; ret.y = vec.y; ret.z = vec.z;
	return ret;
}

inline server::bbox Convert(coords::GlobalBbox bbox)
{
	server::bbox ret;
	ret.min = Convert(bbox.getMin()); ret.max = Convert(bbox.getMax());
	return ret;
}

inline coords::GlobalBbox Convert(server::bbox bbox)
{
	coords::GlobalBbox ret;
	ret.setMin(Convert(bbox.min));
	ret.setMax(Convert(bbox.max));
	return ret;
}

inline server::volType::type Convert(common::ObjectType type)
{
    switch(type) {
    case common::CHANNEL: return server::volType::CHANNEL;
    case common::SEGMENTATION: return server::volType::SEGMENTATION;
    }
    throw argException("Bad volume type.");
}

inline common::ObjectType Convert(server::volType::type type)
{
    switch(type) {
    case server::volType::CHANNEL: return common::CHANNEL;
    case server::volType::SEGMENTATION: return common::SEGMENTATION;
    }
    throw argException("Bad volume type.");
}

inline server::viewType::type Convert(om::common::ViewType type)
{
    switch(type) {
    case common::XY_VIEW: return server::viewType::XY_VIEW;
    case common::XZ_VIEW: return server::viewType::XZ_VIEW;
    case common::ZY_VIEW: return server::viewType::ZY_VIEW;
    }
    throw argException("Bad view type.");
}

inline om::common::ViewType Convert(server::viewType::type type)
{
    switch(type) {
    case server::viewType::XY_VIEW: return common::XY_VIEW;
    case server::viewType::XZ_VIEW: return common::XZ_VIEW;
    case server::viewType::ZY_VIEW: return common::ZY_VIEW;
    }
    throw argException("Bad view type.");
}

inline common::DataType Convert(server::dataType::type type)
{
	switch(type) {
    case server::dataType::INT8: return common::DataType::INT8;
    case server::dataType::UINT8: return common::DataType::UINT8;
    case server::dataType::INT32: return common::DataType::INT32;
    case server::dataType::UINT32: return common::DataType::UINT32;
    case server::dataType::FLOAT: return common::DataType::FLOAT;
	}
	throw argException("Bad data type.");
}

inline server::dataType::type Convert(common::DataType type)
{
	switch(type.index()) {
    case common::DataType::INT8: return server::dataType::INT8;
    case common::DataType::UINT8: return server::dataType::UINT8;
    case common::DataType::INT32: return server::dataType::INT32;
    case common::DataType::UINT32: return server::dataType::UINT32;
    case common::DataType::FLOAT: return server::dataType::FLOAT;
	}
	throw argException("Bad data type.");
}


}} // namespace om::utility::