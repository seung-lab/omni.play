#pragma once

#include "common/common.h"

#include <QtGui>

class MainWindow;
class OmViewGroupState;
class ChannelDataWrapper;
class SegmentationDataWrapper;
namespace om {
namespace gui {
class viewGroupImpl;
}
}

class ViewGroup {
 public:
  ViewGroup(MainWindow*, OmViewGroupState*);
  ~ViewGroup();

  void AddView2Dchannel(const ChannelDataWrapper& cdw,
                        const om::common::ViewType vtype);
  void AddView2Dsegmentation(const SegmentationDataWrapper& sdw,
                             const om::common::ViewType vtype);

  void AddView3D();
  void AddView3D4View();

  void AddXYView();
  void AddAllViews();
  void AddXYViewAndView3d();

 private:
  MainWindow* const mainWindow_;

  std::unique_ptr<om::gui::viewGroupImpl> impl_;
};
