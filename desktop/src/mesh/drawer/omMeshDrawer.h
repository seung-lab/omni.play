#pragma once
#include "precomp.h"

#include "common/common.h"
#include "common/old.hpp"

class OmMeshPlanCache;
class OmSegmentation;
class OmViewGroupState;
class OmVolumeCuller;
class OmMeshSegmentList;
class OmMeshDrawerImpl;

class OmMeshDrawer {
 private:
  OmSegmentation* const segmentation_;
  std::shared_ptr<OmMeshSegmentList> rootSegLists_;
  std::shared_ptr<OmMeshPlanCache> cache_;

  std::shared_ptr<OmVolumeCuller> culler_;
  int numPrevRedraws_;

 public:
  OmMeshDrawer(OmSegmentation*);

  virtual ~OmMeshDrawer() {}

  boost::optional<std::pair<float, float> > Draw(
      OmViewGroupState*, std::shared_ptr<OmVolumeCuller>,
      const OmBitfield drawOptions);

 private:
  void printDrawInfo(const OmMeshDrawerImpl& drawer);
  int getAllowedDrawTime();
  void updateNumPrevRedraws(std::shared_ptr<OmVolumeCuller> culler);
};
