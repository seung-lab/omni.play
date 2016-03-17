#pragma once
#include "precomp.h"

#include "datalayer/hdf5/omExportVolToHdf5.hpp"
#include "gui/widgets/omButton.hpp"
#include "gui/inspectors/channel/exportPage/pageExportChannel.h"

namespace om {
namespace channelInspector {

class ExportButton : public OmButton {
 public:
  ExportButton(QWidget* d, const ChannelDataWrapper& cdw)
      : OmButton(d, "Export", "Export", false), cdw_(cdw) {}

 private:
  const ChannelDataWrapper& cdw_;

  void onLeftClick() override {
    const QString fileName =
        QFileDialog::getSaveFileName(this, tr("Export As"));

    if (fileName == nullptr) return;

    OmExportVolToHdf5::Export(*cdw_.GetChannel(), fileName);
  }
};

}  // namespace channelInspector
}  // namespace om
