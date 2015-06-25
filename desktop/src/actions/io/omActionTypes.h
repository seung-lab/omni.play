#pragma once
#include "precomp.h"

namespace om {
namespace actions_ {

enum ActionTypes {
  OmSegmentValidateAction,
  OmSegmentSplitAction,
  OmSegmentCutAction,
  OmSegmentShatterAction,
  OmSegmentJoinAction,
  OmSegmentSelectAction,
  OmSegmentGroupAction,
  OmSegmentUncertainAction,
  OmVolxelSetvalueAction,
  OmSegmentationThresholdChangeAction,
  OmProjectCloseAction,
  OmProjectSaveAction,
};

typedef std::map<QString, ActionTypes> typeMap_t;

// TODO: use BOOST_ENUM_VALUES?
typeMap_t registerActionTypes();

static const typeMap_t actionStrToType_ = registerActionTypes();

}  // namespace actions_
}  // namespace om
