#pragma once

#include "gui/widgets/omButton.hpp"
#include "gui/inspectors/segmentation/exportPage/pageExport.h"

namespace om {
namespace segmentationInspector {

class ExportButtonRaw : public OmButton<PageExport> {
 public:
  ExportButtonRaw(PageExport* d)
      : OmButton<PageExport>(d, "Export Raw", "Export Raw", false) {}

 private:
  void doAction() {
    const QString fileName =
        QFileDialog::getSaveFileName(this, tr("Export As"));

    if (fileName == nullptr) return;

    const SegmentationDataWrapper& sdw = mParent->GetSDW();

    OmExportVolToHdf5::Export(sdw.GetSegmentation(), fileName, false);
  }
};

}  // namespace segmentationInspector
}  // namespace om
