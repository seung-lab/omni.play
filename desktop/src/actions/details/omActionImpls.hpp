#pragma once
#include "precomp.h"

#include "actions/details/omActionBase.hpp"

#include "actions/details/omProjectCloseActionImpl.hpp"
class OmProjectCloseAction : public OmActionBase<OmProjectCloseActionImpl> {
 public:
  OmProjectCloseAction() { SetUndoable(false); }
};

#include "actions/details/omProjectSaveActionImpl.hpp"
class OmProjectSaveAction : public OmActionBase<OmProjectSaveActionImpl> {
 public:
  OmProjectSaveAction() { SetUndoable(false); }
};

#include "actions/details/omSegmentationThresholdChangeActionImpl.hpp"
class OmSegmentationThresholdChangeAction
    : public OmActionBase<OmSegmentationThresholdChangeActionImpl> {
 public:
  OmSegmentationThresholdChangeAction(
      std::shared_ptr<OmSegmentationThresholdChangeActionImpl> impl) {
    impl_ = impl;
  }

  OmSegmentationThresholdChangeAction(const SegmentationDataWrapper sdw,
                                      const double threshold) {
    impl_ = std::make_shared<OmSegmentationThresholdChangeActionImpl>(
        sdw, threshold);
    SetUndoable(true);
  }
};

#include "actions/details/omSegmentationSizeThresholdChangeActionImpl.hpp"
class OmSegmentationSizeThresholdChangeAction
    : public OmActionBase<OmSegmentationSizeThresholdChangeActionImpl> {
 public:
  OmSegmentationSizeThresholdChangeAction(
      std::shared_ptr<OmSegmentationSizeThresholdChangeActionImpl> impl) {
    impl_ = impl;
  }

  OmSegmentationSizeThresholdChangeAction(const SegmentationDataWrapper sdw,
                                          const double threshold) {
    impl_ = std::make_shared<OmSegmentationSizeThresholdChangeActionImpl>(
        sdw, threshold);
    SetUndoable(true);
  }
};

#include "actions/details/omSegmentSelectActionImpl.hpp"
class OmSegmentSelectAction : public OmActionBase<OmSegmentSelectActionImpl> {
 public:
  OmSegmentSelectAction(std::shared_ptr<OmSegmentSelectActionImpl> impl) {
    SetRedrawParams(impl->GetParams());
    impl_ = impl;
  }

  OmSegmentSelectAction(std::shared_ptr<OmSelectSegmentsParams> params) {
    SetRedrawParams(params);
    impl_ = std::make_shared<OmSegmentSelectActionImpl>(params);
  }
};

#include "actions/details/omSegmentShatterActionImpl.hpp"
class OmSegmentShatterAction : public OmActionBase<OmSegmentShatterActionImpl> {
 public:
  OmSegmentShatterAction(std::shared_ptr<OmSegmentShatterActionImpl> impl) {
    impl_ = impl;
  }

  OmSegmentShatterAction(const SegmentDataWrapper& sdw) {
    impl_ = std::make_shared<OmSegmentShatterActionImpl>(sdw);
    SetUndoable(true);
  }
};

#include "actions/details/omSegmentJoinActionImpl.hpp"
class OmSegmentJoinAction : public OmActionBase<OmSegmentJoinActionImpl> {
 public:
  OmSegmentJoinAction(std::shared_ptr<OmSegmentJoinActionImpl> impl) {
    impl_ = impl;
  }

  OmSegmentJoinAction(const SegmentationDataWrapper& sdw,
                      const om::common::SegIDSet& ids) {
    impl_ = std::make_shared<OmSegmentJoinActionImpl>(sdw, ids);
    SetUndoable(true);
  }
};

#include "actions/details/omSegmentSplitActionImpl.hpp"
class OmSegmentSplitAction : public OmActionBase<OmSegmentSplitActionImpl> {
 public:
  OmSegmentSplitAction(std::shared_ptr<OmSegmentSplitActionImpl> impl) {
    impl_ = impl;
  }

  OmSegmentSplitAction(const SegmentationDataWrapper& sdw,
                      const om::segment::UserEdge& edge) {
    impl_ = std::make_shared<OmSegmentSplitActionImpl>(sdw, edge);
    SetUndoable(true);
  }
};

#include "actions/details/omSegmentMultiSplitActionImpl.hpp"
class OmSegmentMultiSplitAction : public OmActionBase<OmSegmentMultiSplitActionImpl> {
 public:
  OmSegmentMultiSplitAction(std::shared_ptr<OmSegmentMultiSplitActionImpl> impl) {
    impl_ = impl;
  }

  OmSegmentMultiSplitAction(const SegmentationDataWrapper& sdw,
                      const std::vector<om::segment::UserEdge>& edges,
                      const om::common::SegIDSet sources,
                      const om::common::SegIDSet sinks) {
    impl_ = std::make_shared<OmSegmentMultiSplitActionImpl>(sdw, edges, sources, sinks);
    SetUndoable(true);
  }
};
