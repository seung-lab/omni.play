#pragma once

#include "gui/viewGroup/viewGroupMainWindowUtils.hpp"
#include "gui/viewGroup/viewGroupUtils.hpp"

namespace om {
namespace gui {

class viewGroupImpl {
private:
    MainWindow *const mainWindow_;
    OmViewGroupState *const vgs_;

    boost::scoped_ptr<ViewGroupUtils> utils_;
    boost::scoped_ptr<ViewGroupMainWindowUtils> mainWindowUtils_;

public:
    viewGroupImpl(MainWindow* mainWindow, OmViewGroupState* vgs)
        : mainWindow_(mainWindow)
        , vgs_(vgs)
        , utils_(new ViewGroupUtils(mainWindow, vgs))
        , mainWindowUtils_(new ViewGroupMainWindowUtils(mainWindow, vgs, utils_.get()))
    {}

    ~viewGroupImpl()
    {}

    void AddView3D()
    {
        ViewGroupWidgetInfo vgw = utils_->CreateView3d();
        mainWindowUtils_->InsertDockIntoGroup4View(vgw);
    }

    void addView3D2View(QDockWidget* widget)
    {
        ViewGroupWidgetInfo vgw = utils_->CreateView3d();
        mainWindowUtils_->InsertBySplitting(vgw, widget);
        QApplication::processEvents();
    }

    void AddView3D4View()
    {
        ViewGroupWidgetInfo vgw = utils_->CreateView3d();
        mainWindowUtils_->InsertDockIntoGroup4View(vgw);
    }

    QDockWidget* AddView2Dchannel(const ChannelDataWrapper& cdw, const ViewType viewType)
    {
        ViewGroupWidgetInfo vgw = utils_->CreateView2dChannel(cdw, viewType);

        std::pair<QDockWidget*, QDockWidget*> dockAndCompliment =
            mainWindowUtils_->InsertDockIntoGroup4View(vgw);

        utils_->wireDockWithView2d(vgw.widget, dockAndCompliment);

        return dockAndCompliment.first;
    }

    std::pair<QDockWidget*, QDockWidget*>
    AddView2Dsegmentation(const SegmentationDataWrapper& sdw, const ViewType viewType)
    {
        ViewGroupWidgetInfo vgw = utils_->CreateView2dSegmentation(sdw, viewType);

        std::pair<QDockWidget*, QDockWidget*> dockAndCompliment =
            mainWindowUtils_->InsertDockIntoGroup4View(vgw);

        utils_->wireDockWithView2d(vgw.widget, dockAndCompliment);

        return dockAndCompliment;
    }

    void AddXYView()
    {
        Q_FOREACH(QDockWidget* w, utils_->getAllDockWidgets()){
            delete w;
        }

        const ChannelDataWrapper cdw = vgs_->Channel();
        const SegmentationDataWrapper sdw = vgs_->Segmentation();

        if(utils_->canShowChannel(cdw)){
            AddView2Dchannel(cdw, XY_VIEW);
        }

        if(utils_->canShowSegmentation(sdw)){
            AddView2Dsegmentation(sdw, XY_VIEW);
        }
    }

    void AddAllViews()
    {
        Q_FOREACH(QDockWidget* w, utils_->getAllDockWidgets()){
            delete w;
        }

        const ChannelDataWrapper cdw = vgs_->Channel();
        const SegmentationDataWrapper sdw = vgs_->Segmentation();

        const bool validChan = utils_->canShowChannel(cdw);
        const bool validSeg = utils_->canShowSegmentation(sdw);

        if(validChan && validSeg)
        {
            AddView2Dchannel(cdw, mainWindowUtils_->UpperLeft());
            AddView2Dchannel(cdw, mainWindowUtils_->UpperRight());
            AddView2Dchannel(cdw, mainWindowUtils_->LowerLeft());

            AddView2Dsegmentation(sdw, mainWindowUtils_->UpperLeft());

            std::pair<QDockWidget*, QDockWidget*> dockAndCompliment =
                AddView2Dsegmentation(sdw, mainWindowUtils_->UpperRight());

            AddView2Dsegmentation(sdw, mainWindowUtils_->LowerLeft());

            AddView3D4View();

            // raise must be done after adding view3d (Qt timing reasons?)
            dockAndCompliment.second->raise();

        } else {
            if(validChan){
                AddView2Dchannel(cdw, mainWindowUtils_->UpperLeft());
                AddView2Dchannel(cdw, mainWindowUtils_->UpperRight());
                AddView2Dchannel(cdw, mainWindowUtils_->LowerLeft());
            }
            if(validSeg){
                AddView2Dsegmentation(sdw, mainWindowUtils_->UpperLeft());
                AddView2Dsegmentation(sdw, mainWindowUtils_->UpperRight());
                AddView2Dsegmentation(sdw, mainWindowUtils_->LowerLeft());
                AddView3D4View();
            }
        }
    }

    void AddXYViewAndView3d()
    {
        Q_FOREACH(QDockWidget* w, utils_->getAllDockWidgets()){
            delete w;
        }

        const ChannelDataWrapper cdw = vgs_->Channel();
        const SegmentationDataWrapper sdw = vgs_->Segmentation();

        const bool showChannel = utils_->canShowChannel(cdw);
        const ViewType viewType = XY_VIEW;

        QDockWidget* dockToSplit = NULL;
        if(showChannel){
            dockToSplit = AddView2Dchannel(cdw, viewType);
        }

        if(!utils_->canShowSegmentation(sdw)){
            return;
        }

        if(dockToSplit)
        {
            addView3D2View(dockToSplit);
            AddView2Dsegmentation(sdw, viewType);

        } else {
            std::pair<QDockWidget*, QDockWidget*> dockAndCompliment =
                AddView2Dsegmentation(sdw, viewType);

            dockToSplit = dockAndCompliment.first;
            addView3D2View(dockToSplit);
        }
    }

};

} // namespace viewGroup
} // namespace om
