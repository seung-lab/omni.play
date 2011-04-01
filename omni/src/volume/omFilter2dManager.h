#ifndef OM_FILTER_MANAGER_H
#define OM_FILTER_MANAGER_H

/*
 *  Matt Wimer mwimer@mit.edu Oct13, 2009
 */

#include "common/omCommon.h"
#include "system/omGenericManager.hpp"

class OmFilter2d;

class OmFilter2dManager {
public:
    OmFilter2dManager();

    inline OmFilter2d& GetFilter(OmID id){
        return filters_.Get(id);
    }

    inline const OmIDsSet& GetValidFilterIds() const {
        return filters_.GetValidIds();
    }

    inline bool IsFilterEnabled(OmID id) const {
        return filters_.IsEnabled(id);
    }

    inline bool IsFilterValid(OmID id) const {
        return filters_.IsValid(id);
    }

    OmFilter2d& AddFilter();

    inline std::vector<OmFilter2d*> GetFilters() const {
        return filters_.GetPtrVec();
    }

private:
    OmGenericManager<OmFilter2d> filters_;

    friend QDataStream &operator<<(QDataStream&, const OmFilter2dManager&);
    friend QDataStream &operator>>(QDataStream&, OmFilter2dManager& );
};

#endif
