#pragma once

#include <QtGui>

class SegmentationDataWrapper;

namespace om {
namespace segmentationInspector {

class PagesWidget : public QStackedWidget {
 public:
  PagesWidget(QWidget* parent, const SegmentationDataWrapper& sdw);
};

}  // namespace segmentationInspector
}  // namespace om
