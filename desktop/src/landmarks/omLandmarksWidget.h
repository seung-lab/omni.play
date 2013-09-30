#pragma once

#include "landmarks/omLandmarksTypes.h"

#include <QVBoxLayout>

namespace om {
namespace landmarks {

class dialog;
class widget;

class widget : public QWidget {
 private:
  dialog *const dialog_;
  const std::vector<sdwAndPt> pts_;

 public:
  widget(dialog *const menu, const std::vector<sdwAndPt> &pts);

  void Hide();

 private:
  void create();
  QString showSegmentList();
};

}  // namespace landmarks
}  // namespace om
