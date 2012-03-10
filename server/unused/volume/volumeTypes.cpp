#include "volume/volumeTypes.h"

using namespace om::common;

namespace om {
namespace volume {

std::string typeHelpers::GetTypeAsString(const dataType type){
    return type.value();
}

dataType typeHelpers::GetTypeFromString(const std::string & type)
{
    boost::optional<dataType> ret = dataType::get_by_value(type);

    if(ret){
        return *ret;
    }

    throw common::ioException(str(boost::format("invalid type %1%") % type));
}

} // namespace volume
} // namespace om
