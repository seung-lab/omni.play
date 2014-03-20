#include "utility/segmentationDataWrapper.hpp"
#include "volume/omSegmentation.h"
#include "sizeThresholdSpinBox.hpp"

boost::optional<double> SizeThresholdSpinBox::getCurVolThreshold() {
  auto sdw = mParent->GetSDW();

  if (!sdw.IsSegmentationValid()) {
    return boost::optional<double>();
  }

  return sdw.GetSegmentation()->GetSizeThreshold();
}

void SizeThresholdSpinBox::actUponValueChange(const double newThreshold) {
  auto threshold = getCurVolThreshold();
  if (!threshold) {
    return;
  }

  if (qFuzzyCompare(*threshold, newThreshold)) {
    return;
  }

  OmActions::ChangeSizethreshold(mParent->GetSDW(), newThreshold);
}
