#ifndef OM_FILTER_MANAGER_H
#define OM_FILTER_MANAGER_H

/*
 *  Matt Wimer mwimer@mit.edu Oct13, 2009
 */

#include "common/omCommon.h"
#include "system/omGenericManager.h"

class OmFilter2d;

class OmFilter2dManager {
public:
    OmFilter2dManager();

    OmFilter2d& AddFilter();
    OmFilter2d& GetFilter(OmID id);
    const OmIDsSet & GetValidFilterIds();
    bool IsFilterEnabled(OmID id);
    bool IsFilterValid(OmID id);

    std::vector<OmFilter2d*> GetFilters();

private:
    OmGenericManager<OmFilter2d> filters_;

    friend QDataStream &operator<<(QDataStream&, const OmFilter2dManager&);
    friend QDataStream &operator>>(QDataStream&, OmFilter2dManager& );
};

#endif
