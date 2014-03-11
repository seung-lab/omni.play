#pragma once

#include "events/listeners.h"
#include "gui/widgets/omIntSpinBox.hpp"
#include "utility/dataWrappers.h"
#include "view2d/omView2dConverters.hpp"
#include "viewGroup/omViewGroupState.h"
#include "viewGroup/omViewGroupView2dState.hpp"
#include "volume/omMipVolume.h"
#include "volume/omSegmentation.h"

#include <limits>

class SliceDepthSpinBoxBase : public OmIntSpinBox,
                              public om::event::View2dEventListener {
  Q_OBJECT;

 public:
  SliceDepthSpinBoxBase(QWidget* d, OmViewGroupState* vgs)
      : OmIntSpinBox(d, true), vgs_(vgs) {
    setValue(0);
    setSingleStep(1);
    setMaximum(std::numeric_limits<int32_t>::max());
  }

  QSize sizeHint() const { return QSize(50, height()); }

  virtual QString Label() const = 0;

 private:
  OmViewGroupState* const vgs_;

  virtual om::common::ViewType viewType() const = 0;

  OmMipVolume* getVol() {
    {
      const ChannelDataWrapper cdw = vgs_->Channel();
      if (cdw.IsValidWrapper()) {
        return cdw.GetChannel();
      }
    }

    {
      const SegmentationDataWrapper sdw = vgs_->Segmentation();
      if (sdw.IsValidWrapper()) {
        return sdw.GetSegmentation();
      }
    }

    return nullptr;
  }

  void actUponValueChange(const int depth) {
    if (nullptr == vg2ds()) {
      return;
    }

    vg2ds()->SetScaledSliceDepth(viewType(), depth);
    om::event::Redraw2d();
    om::event::ViewCenterChanged();
    om::event::View3dRecenter();
  }

  void update() {
    blockSignals(true);
    const int depth = vg2ds()->GetScaledSliceDepth(viewType());
    setValue(depth);
    blockSignals(false);
  }

  inline OmViewGroupView2dState* vg2ds() const { return vgs_->View2dState(); }

  // OmViewEventListener
  void ViewBoxChangeEvent() { update(); }
  void ViewCenterChangeEvent() { update(); }
  void CoordSystemChangeEvent() { update(); }
  void ViewPosChangeEvent() {}
  void ViewRedrawEvent() {}
  void ViewBlockingRedrawEvent() {}
};
