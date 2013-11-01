#pragma once

#include "gui/widgets/progressBarDialog.hpp"
#include "gui/inspectors/segmentation/buildPage/segVolBuilder.hpp"
#include "gui/inspectors/segmentation/buildPage/sourceBox.hpp"

namespace om {
namespace segmentationInspector {

class PageBuilder : public QWidget {
  Q_OBJECT;
  ;
 private:
  const SegmentationDataWrapper sdw_;

  SourceBox* const sourceBox_;
  QComboBox* buildComboBox_;

 public:
  PageBuilder(QWidget* parent, const SegmentationDataWrapper& sdw);

 private
Q_SLOTS:
  void build();

 private:
  static void doBuild(const SegmentationDataWrapper sdw,
                      const QString whatOrHowToBuild,
                      const QFileInfoList fileNamesAndPaths,
                      om::gui::progressBarDialog* dialog);

  QGridLayout* makeActionsBox();
};

}  // namespace segmentationInspector
}  // namespace om
