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

    QDockWidget* AddView2Dchannel(const OmID chanID, const ViewType viewType)
    {
        ViewGroupWidgetInfo vgw = utils_->CreateView2dChannel(chanID, viewType);

        std::pair<QDockWidget*, QDockWidget*> dockAndCompliment =
            mainWindowUtils_->InsertDockIntoGroup4View(vgw);

        utils_->wireDockWithView2d(vgw.widget, dockAndCompliment);

        return dockAndCompliment.first;
    }

    std::pair<QDockWidget*, QDockWidget*>
    AddView2Dsegmentation(const OmID segmentationID, const ViewType viewType)
    {
        ViewGroupWidgetInfo vgw = utils_->CreateView2dSegmentation(segmentationID, viewType);

        std::pair<QDockWidget*, QDockWidget*> dockAndCompliment =
            mainWindowUtils_->InsertDockIntoGroup4View(vgw);

        utils_->wireDockWithView2d(vgw.widget, dockAndCompliment);

        return dockAndCompliment;
    }

    void AddXYView(const OmID channelID, const OmID segmentationID)
    {
        Q_FOREACH(QDockWidget* w, utils_->getAllDockWidgets()){
            delete w;
        }

        if(utils_->canShowChannel(channelID)){
            AddView2Dchannel(channelID, XY_VIEW);
        }

        if(utils_->canShowSegmentation(segmentationID)){
            AddView2Dsegmentation(segmentationID, XY_VIEW);
        }
    }

    void AddAllViews(const OmID channelID, const OmID segmentationID)
    {
        Q_FOREACH(QDockWidget* w, utils_->getAllDockWidgets()){
            delete w;
        }

        const bool validChan = utils_->canShowChannel(channelID);
        const bool validSeg = utils_->canShowSegmentation(segmentationID);

        if(validChan && validSeg)
        {
            AddView2Dchannel(channelID, mainWindowUtils_->UpperLeft());
            AddView2Dchannel(channelID, mainWindowUtils_->UpperRight());
            AddView2Dchannel(channelID, mainWindowUtils_->LowerLeft());

            AddView2Dsegmentation(segmentationID, mainWindowUtils_->UpperLeft());

            std::pair<QDockWidget*, QDockWidget*> dockAndCompliment =
                AddView2Dsegmentation(segmentationID, mainWindowUtils_->UpperRight());

            AddView2Dsegmentation(segmentationID, mainWindowUtils_->LowerLeft());

            AddView3D4View();

            // raise must be done after adding view3d (Qt timing reasons?)
            dockAndCompliment.second->raise();

        } else {
            if(validChan){
                AddView2Dchannel(channelID, mainWindowUtils_->UpperLeft());
                AddView2Dchannel(channelID, mainWindowUtils_->UpperRight());
                AddView2Dchannel(channelID, mainWindowUtils_->LowerLeft());
            }
            if(validSeg){
                AddView2Dsegmentation(segmentationID, mainWindowUtils_->UpperLeft());
                AddView2Dsegmentation(segmentationID, mainWindowUtils_->UpperRight());
                AddView2Dsegmentation(segmentationID, mainWindowUtils_->LowerLeft());
                AddView3D4View();
            }
        }
    }

    void AddXYViewAndView3d(const OmID channelID, const OmID segmentationID)
    {
        Q_FOREACH(QDockWidget* w, utils_->getAllDockWidgets()){
            delete w;
        }

        const bool showChannel = utils_->canShowChannel(channelID);
        const ViewType viewType = XY_VIEW;

        QDockWidget* dockToSplit = NULL;
        if(showChannel){
            dockToSplit = AddView2Dchannel(channelID, viewType);
        }

        if(!utils_->canShowSegmentation(segmentationID)){
            return;
        }

        if(dockToSplit)
        {
            addView3D2View(dockToSplit);
            AddView2Dsegmentation(segmentationID, viewType);

        } else {
            std::pair<QDockWidget*, QDockWidget*> dockAndCompliment =
                AddView2Dsegmentation(segmentationID, viewType);
            dockToSplit = dockAndCompliment.first;
            addView3D2View(dockToSplit);
        }
    }

};

} // namespace viewGroup
} // namespace om
