#pragma once
#include "precomp.h"

#include "gui/widgets/segmentationDataWrapperButton.hpp"
#include "gui/inspectors/segmentation/exportPage/pageExport.h"
#include "gui/widgets/segmentationDataWrapperButton.hpp"

namespace om {
namespace segmentationInspector {

class ExportButtonRaw : public OmButton {
 public:
  ExportButtonRaw(QWidget* widget)
      : SegmentationDataWrapperButton(widget, "Export Raw", "Export Raw", false) {}

 private:
  void onLeftClick() override {
    const QString fileName =
        QFileDialog::getSaveFileName(this, tr("Export As"));

    if (fileName == nullptr) return;

    const SegmentationDataWrapper& sdw = mParent->GetSDW();

    OmExportVolToHdf5::Export(sdw.GetSegmentation(), fileName, false);
  }
};

}  // namespace segmentationInspector
}  // namespace om
