#pragma once
#include "precomp.h"

#include "gui/widgets/omButton.hpp"
#include "utility/segmentationDataWrapper.hpp"

class SegmentationDataWrapperButton : public OmButton {
 public:
  SegmentationDataWrapperButton(QWidget* mw, const QString& title,
      const QString& statusTip, const bool isCheckable,
      const SegmentationDataWrapper& segmentationDataWrapper)
      : OmButton(mw, title, statusTip, isCheckable),
        segmentationDataWrapper_(segmentationDataWrapper) {};

  const SegmentationDataWrapper& GetSegmentationDataWrapper() const {
    return segmentationDataWrapper_;
  }

 private:
  const SegmentationDataWrapper& segmentationDataWrapper_;
};
