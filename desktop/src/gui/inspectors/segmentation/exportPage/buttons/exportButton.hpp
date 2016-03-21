#pragma once
#include "precomp.h"

#include "datalayer/hdf5/omExportVolToHdf5.hpp"
#include "gui/widgets/segmentationDataWrapperButton.hpp"
#include "gui/inspectors/segmentation/exportPage/pageExport.h"
#include "gui/widgets/segmentationDataWrapperButton.hpp"

namespace om {
namespace segmentationInspector {

class ExportButton : public SegmentationDataWrapperButton {
 public:
  ExportButton(QWidget* widget,
      const SegmentationDataWrapper& segmentationDataWrapper, bool rerootSegments)
      : SegmentationDataWrapperButton(widget,
         rerootSegments ? "Export and reroot segments" : "Export Raw",
         rerootSegments ? "Export" : "Export Raw",
         false, segmentationDataWrapper),
        rerootSegments_(rerootSegments) {
  }

 private:
  bool rerootSegments_;

  void onLeftClick() override {
    const QString fileName =
        QFileDialog::getSaveFileName(this, tr("Export As"));

    if (fileName == nullptr) return;

    const SegmentationDataWrapper& sdw = GetSegmentationDataWrapper();

    OmExportVolToHdf5::Export(sdw.GetSegmentation(), fileName, rerootSegments_);
  }
};

}  // namespace segmentationInspector
}  // namespace om
