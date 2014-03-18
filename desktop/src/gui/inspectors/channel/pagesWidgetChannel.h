#pragma once
#include "precomp.h"

class ChannelDataWrapper;

namespace om {
namespace channelInspector {

class PagesWidget : public QStackedWidget {
 public:
  PagesWidget(QWidget* parent, const ChannelDataWrapper& cdw);
};

}  // namespace segmentationInspector
}  // namespace om
