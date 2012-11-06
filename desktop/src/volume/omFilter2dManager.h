#pragma once

#include "common/common.h"
#include "common/genericManager.hpp"
#include "datalayer/archive/filter.h"

#include <QDataStream>

class OmFilter2d;

class OmFilter2dManager {
public:
    OmFilter2dManager();

    inline OmFilter2d& GetFilter(om::common::ID id){
        return filters_.Get(id);
    }

    inline const om::common::IDSet& GetValidFilterIds() const {
        return filters_.GetValidIds();
    }

    inline bool IsFilterEnabled(om::common::ID id) const {
        return filters_.IsEnabled(id);
    }

    inline bool IsFilterValid(om::common::ID id) const {
        return filters_.IsValid(id);
    }

    OmFilter2d& AddFilter();

    inline std::vector<OmFilter2d*> GetFilters() const {
        return filters_.GetPtrVec();
    }

private:
    om::common::GenericManager<OmFilter2d> filters_;

    friend YAML::Emitter &YAML::operator<<(YAML::Emitter&, const OmFilter2dManager&);
    friend void YAML::operator>>(const YAML::Node&, OmFilter2dManager& );
    friend QDataStream &operator<<(QDataStream&, const OmFilter2dManager&);
    friend QDataStream &operator>>(QDataStream&, OmFilter2dManager& );
};

