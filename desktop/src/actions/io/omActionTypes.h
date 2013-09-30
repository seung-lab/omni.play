#pragma once

#include <map>
#include <QString>

namespace om {
namespace actions_ {

enum ActionTypes {
  OmSegmentValidateAction,
  OmSegmentSplitAction,
  OmSegmentCutAction,
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
