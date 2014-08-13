#pragma once
#include "precomp.h"

/*
 *  Matt Wimer mwimer@mit.edu Oct13, 2009
 */

#include "common/common.h"
#include "system/omGenericManager.hpp"
#include "datalayer/archive/filter.h"

class OmFilter2d;

class OmFilter2dManager {
 public:
  OmFilter2dManager();

  inline OmFilter2d* GetFilter(om::common::ID id) { return filters_.Get(id); }

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
  OmGenericManager<OmFilter2d> filters_;

  friend class YAML::convert<OmFilter2dManager>;
  friend QDataStream& operator<<(QDataStream&, const OmFilter2dManager&);
  friend QDataStream& operator>>(QDataStream&, OmFilter2dManager&);
};
