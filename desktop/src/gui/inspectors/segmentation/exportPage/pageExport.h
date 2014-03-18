#pragma once
#include "precomp.h"

#include "utility/segmentationDataWrapper.hpp"

namespace om {
namespace segmentationInspector {

class PageExport : public QWidget {
 private:
  const SegmentationDataWrapper sdw_;

 public:
  PageExport(QWidget* parent, const SegmentationDataWrapper& sdw);

  const SegmentationDataWrapper& GetSDW() const { return sdw_; }

 private:
  QPlainTextEdit* notesEdit_;

  QGroupBox* makeActionsBox();
};

}  // namespace segmentationInspector
}  // namespace om
