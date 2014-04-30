#pragma once
#include "precomp.h"

#include "system/omConnect.hpp"
#include "gui/viewGroup/viewGroupMainWindowUtils.hpp"
#include "gui/viewGroup/viewGroupUtils.hpp"

namespace om {
namespace gui {

class viewGroupImpl : public QObject {
  Q_OBJECT;

 private:
  OmViewGroupState& vgs_;

  std::unique_ptr<ViewGroupUtils> utils_;
  std::unique_ptr<ViewGroupMainWindowUtils> mainWindowUtils_;

 public:
  viewGroupImpl(MainWindow* mainWindow, OmViewGroupState& vgs)
      : vgs_(vgs),
        utils_(new ViewGroupUtils(mainWindow, vgs)),
        mainWindowUtils_(new ViewGroupMainWindowUtils(mainWindow,
                                                      utils_.get())) {
    om::connect(this, SIGNAL(signalAddView3D()), this, SLOT(addView3D()));

    om::connect(this, SIGNAL(signalAddView3D2View(const om::common::ViewType)),
                this, SLOT(addView3D2View(const om::common::ViewType)));

    om::connect(this, SIGNAL(signalAddView3D4View()), this,
                SLOT(addView3D4View()));

    om::connect(
        this, SIGNAL(signalAddView2Dchannel(ChannelDataWrapper,
                                            om::common::ViewType)),
        this, SLOT(addView2Dchannel(ChannelDataWrapper, om::common::ViewType)));

    om::connect(this, SIGNAL(signalAddView2Dsegmentation(
                          SegmentationDataWrapper, om::common::ViewType)),
                this, SLOT(addView2Dsegmentation(SegmentationDataWrapper,
                                                 om::common::ViewType)));
  }

  ~viewGroupImpl() {}

 private
Q_SLOTS:

  void addView3D() {
    ViewGroupWidgetInfo vgw = utils_->CreateView3d();
    mainWindowUtils_->InsertDockIntoGroup4View(vgw);
  }

  void addView3D2View(const om::common::ViewType viewType) {
    ViewGroupWidgetInfo vgw = utils_->CreateView3d();
    mainWindowUtils_->InsertDockIntoGroup2View(vgw, viewType);
  }

  void addView3D4View() {
    ViewGroupWidgetInfo vgw = utils_->CreateView3d();
    mainWindowUtils_->InsertDockIntoGroup4View(vgw);
  }

  void addView2Dchannel(const ChannelDataWrapper& cdw,
                        const om::common::ViewType viewType) {
    ViewGroupWidgetInfo vgw = utils_->CreateView2dChannel(cdw, viewType);

    DockWidgetPair d = mainWindowUtils_->InsertDockIntoGroup4View(vgw);

    utils_->wireDockWithView2d(vgw.widget, d);
  }

  void addView2Dsegmentation(const SegmentationDataWrapper& sdw,
                             const om::common::ViewType viewType) {
    ViewGroupWidgetInfo vgw = utils_->CreateView2dSegmentation(sdw, viewType);

    DockWidgetPair d = mainWindowUtils_->InsertDockIntoGroup4View(vgw);

    utils_->wireDockWithView2d(vgw.widget, d);
  }

Q_SIGNALS:
  void signalAddView3D();
  void signalAddView3D2View(const om::common::ViewType viewType);
  void signalAddView3D4View();
  void signalAddView2Dchannel(const ChannelDataWrapper& cdw,
                              const om::common::ViewType viewType);
  void signalAddView2Dsegmentation(const SegmentationDataWrapper& sdw,
                                   const om::common::ViewType viewType);

 private:
  void deleteWidgets() {
    Q_FOREACH(QDockWidget * w, utils_->getAllDockWidgets()) {
      w->hide();
      w->deleteLater();
    }
  }

  void showChannel3View(const ChannelDataWrapper& cdw) {
    signalAddView2Dchannel(cdw, mainWindowUtils_->UpperLeft());
    signalAddView2Dchannel(cdw, mainWindowUtils_->UpperRight());
    signalAddView2Dchannel(cdw, mainWindowUtils_->LowerLeft());
  }

  void showSegmentation3View(const SegmentationDataWrapper& sdw) {
    signalAddView2Dsegmentation(sdw, mainWindowUtils_->UpperLeft());
    signalAddView2Dsegmentation(sdw, mainWindowUtils_->UpperRight());
    signalAddView2Dsegmentation(sdw, mainWindowUtils_->LowerLeft());
  }

 public:
  void AddView3D() { signalAddView3D(); }

  void AddView3D4View() { signalAddView3D4View(); }

  void AddView2Dchannel(const ChannelDataWrapper& cdw,
                        const om::common::ViewType viewType) {
    signalAddView2Dchannel(cdw, viewType);
  }

  void AddView2Dsegmentation(const SegmentationDataWrapper& sdw,
                             const om::common::ViewType viewType) {
    signalAddView2Dsegmentation(sdw, viewType);
  }

  void AddXYView() {
    deleteWidgets();

    static const om::common::ViewType viewType = om::common::XY_VIEW;

    const ChannelDataWrapper cdw = vgs_.Channel();
    if (utils_->canShowChannel(cdw)) {
      signalAddView2Dchannel(cdw, viewType);
    }

    const SegmentationDataWrapper sdw = vgs_.Segmentation();
    if (utils_->canShowSegmentation(sdw)) {
      signalAddView2Dsegmentation(sdw, viewType);
    }
  }

  void AddAllViews() {
    deleteWidgets();

    const ChannelDataWrapper cdw = vgs_.Channel();
    const bool validChan = utils_->canShowChannel(cdw);

    const SegmentationDataWrapper sdw = vgs_.Segmentation();
    const bool validSeg = utils_->canShowSegmentation(sdw);

    if (validChan && validSeg) {
      showChannel3View(cdw);
      signalAddView3D4View();
      showSegmentation3View(sdw);

    } else {

      if (validChan) {
        showChannel3View(cdw);
      }

      if (validSeg) {
        showSegmentation3View(sdw);
        signalAddView3D4View();
      }
    }
  }

  void AddXYViewAndView3d() {
    deleteWidgets();

    static const om::common::ViewType viewType = om::common::XY_VIEW;

    const ChannelDataWrapper cdw = vgs_.Channel();
    const bool showChannel = utils_->canShowChannel(cdw);

    if (showChannel) {
      signalAddView2Dchannel(cdw, viewType);
    }

    const SegmentationDataWrapper sdw = vgs_.Segmentation();

    if (!utils_->canShowSegmentation(sdw)) {
      return;
    }

    if (showChannel) {
      signalAddView3D2View(viewType);
      signalAddView2Dsegmentation(sdw, viewType);

    } else {
      signalAddView2Dsegmentation(sdw, viewType);
      signalAddView3D2View(viewType);
    }
  }
};

}  // namespace gui
}  // namespace om
