#pragma once
#include "precomp.h"

#include "utility/channelDataWrapper.hpp"

namespace om {
namespace channelInspector {

class PageExport : public QWidget {
 private:
  const ChannelDataWrapper cdw_;

 public:
  PageExport(QWidget* parent, const ChannelDataWrapper& cdw);

  const ChannelDataWrapper& GetCDW() const { return cdw_; }

 private:
  QGroupBox* makeExportBox();
};

}  // namespace channelInspector
}  // namespace om
