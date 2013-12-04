#pragma once

#include "datalayer/hdf5/omExportVolToHdf5.hpp"
#include "gui/widgets/omButton.hpp"
#include "gui/inspectors/channel/exportPage/pageExportChannel.h"

namespace om {
namespace channelInspector {

class ExportButton : public OmButton<PageExport> {
 public:
  ExportButton(PageExport* d)
      : OmButton<PageExport>(d, "Export", "Export", false) {}

 private:
  void doAction() {
    const QString fileName =
        QFileDialog::getSaveFileName(this, tr("Export As"));

    if (fileName == nullptr) return;

    const ChannelDataWrapper& cdw = mParent->GetCDW();

    OmExportVolToHdf5::Export(cdw.GetChannel(), fileName);
  }
};

}  // namespace channelInspector
}  // namespace om
