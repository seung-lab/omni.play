#pragma once
#include "precomp.h"

#include "landmarks/omLandmarksTypes.h"

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
