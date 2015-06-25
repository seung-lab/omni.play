#include "actions/io/omActionTypes.h"

om::actions_::typeMap_t om::actions_::registerActionTypes() {
  om::actions_::typeMap_t ret;

  ret["OmSegmentValidateAction"] = OmSegmentValidateAction;
  ret["OmSegmentSplitAction"] = OmSegmentSplitAction;
  ret["OmSegmentCutAction"] = OmSegmentCutAction;
  ret["OmSegmentShatterAction"] = OmSegmentShatterAction;
  ret["OmSegmentJoinAction"] = OmSegmentJoinAction;
  ret["OmSegmentSelectAction"] = OmSegmentSelectAction;
  ret["OmSegmentGroupAction"] = OmSegmentGroupAction;
  ret["OmSegmentUncertainAction"] = OmSegmentUncertainAction;
  ret["OmVolxelSetvalueAction"] = OmVolxelSetvalueAction;
  ret["OmSegmentationThresholdChangeAction"] =
      OmSegmentationThresholdChangeAction;
  ret["OmProjectCloseAction"] = OmProjectCloseAction;
  ret["OmProjectSaveAction"] = OmProjectSaveAction;

  return ret;
}
