#pragma once
#include "precomp.h"

#include "volume/omAffinityGraph.h"
#include "project/omProject.h"
#include "project/details/omProjectVolumes.h"
#include "project/details/omAffinityGraphManager.h"

class AffinityGraphDataWrapper {
 public:
  static const om::common::IDSet& ValidIDs() {
    return OmProject::Volumes().AffinityGraphs().GetValidIds();
  }

 private:
  om::common::ID id_;

 public:
  AffinityGraphDataWrapper() : id_(0) {}

  explicit AffinityGraphDataWrapper(const om::common::ID ID) : id_(ID) {}

  inline om::common::ID GetID() const { return id_; }

  inline bool IsValidWrapper() const { return IsAffinityGraphValid(); }

  inline bool IsAffinityGraphValid() const {
    if (!id_) {
      return false;
    }
    return OmProject::Volumes().AffinityGraphs().IsValid(id_);
  }

  inline OmAffinityGraph* GetAffinityGraph() const {
    return OmProject::Volumes().AffinityGraphs().Get(id_);
  }

  inline QString GetName() const {
    if (!IsValidWrapper()) {
      return "";
    }
    return QString::fromStdString(GetAffinityGraph()->GetName());
  }

  inline bool isEnabled() const {
    return OmProject::Volumes().AffinityGraphs().IsEnabled(id_);
  }

  OmAffinityGraph& Create() {
    auto& a = OmProject::Volumes().AffinityGraphs().Add();
    id_ = a.GetID();
    log_infos << "create affinity graph " << id_;
    return a;
  }
};
