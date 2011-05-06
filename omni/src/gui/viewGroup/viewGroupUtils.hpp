#pragma once

#include "common/omCommon.h"
#include "gui/mainWindow/mainWindow.h"
#include "gui/viewGroup/viewGroupWidgetInfo.h"
#include "gui/widgets/omTellInfo.hpp"
#include "utility/dataWrappers.h"
#include "view2d/omView2d.h"
#include "view3d/omView3d.h"
#include "viewGroup/omViewGroupState.h"

#include <QtGui>

class ViewGroupUtils {
private:
    MainWindow *const mainWindow_;
    OmViewGroupState *const vgs_;

public:
    ViewGroupUtils(MainWindow* mainWindow, OmViewGroupState* vgs)
        : mainWindow_(mainWindow)
        , vgs_(vgs)
    {}

    ~ViewGroupUtils()
    {}

    int getID();

    QList<QDockWidget*> findDockWidgets(const QString& name);
    QList<QDockWidget*> findDockWidgets(const QRegExp& regExp);

    QString viewGroupName(){
        return "ViewGroup" + QString::number(getID());
    }

    QString makeObjectName(){
        return "3d_" + viewGroupName();
    }

    QString getViewName(const std::string& volName, const ViewType viewType)
    {
        return QString::fromStdString(volName)
            + " -- "
            + getViewTypeAsStr(viewType)
            + " View";
    }

    QString getViewTypeAsStr(const ViewType viewType)
    {
        switch(viewType){
        case XY_VIEW:
            return "XY";
        case XZ_VIEW:
            return "XZ";
        case ZY_VIEW:
            return "ZY";
        default:
            throw OmArgException("unknown viewtype");
        }
    }

    QString makeObjectName(const ObjectType voltype, const ViewType viewType)
    {
        QString name;

        if( CHANNEL == voltype ){
            name = "channel_";
        } else if ( SEGMENTATION == voltype ) {
            name = "segmentation_";
        }

        name += getViewTypeAsStr(viewType) + "_" + viewGroupName();

        return name;
    }

    QString makeObjectName(ViewGroupWidgetInfo& vgw)
    {
        if( VIEW2D_CHAN == vgw.widgetType ) {
            return makeObjectName( CHANNEL, vgw.viewType );
        }

        if (VIEW2D_SEG  == vgw.widgetType ){
            return makeObjectName( SEGMENTATION, vgw.viewType );
        }

        return makeObjectName();
    }

    QString makeComplimentaryObjectName(ViewGroupWidgetInfo& vgw)
    {
        if( VIEW2D_CHAN == vgw.widgetType ){
            return makeObjectName( SEGMENTATION, vgw.viewType );
        }

        if( VIEW2D_SEG == vgw.widgetType ){
            return makeObjectName( CHANNEL, vgw.viewType );
        }

        return "";
    }

    QDockWidget* getDockWidget(const ObjectType voltype, const ViewType viewType){
        return getDockWidget(makeObjectName(voltype, viewType));
    }

    QDockWidget* getDockWidget(const QString& objName)
    {
        QList<QDockWidget*> widgets = findDockWidgets(objName);

        if( widgets.isEmpty() ){
            return NULL;
        } else if( widgets.size() > 1 ){
            throw OmArgException("too many widgets");
        }

        return widgets[0];
    }

    bool doesDockWidgetExist(const ObjectType voltype, const ViewType viewType){
        return doesDockWidgetExist(makeObjectName(voltype, viewType));
    }

    bool doesDockWidgetExist(const QString& objName )
    {
        QList<QDockWidget*> widgets = findDockWidgets(objName);

        if( widgets.isEmpty() ){
            return false;
        }

        return true;
    }

    QList<QDockWidget*> getAllDockWidgets()
    {
        QRegExp rx( ".*" + viewGroupName() + "$" );
        QList<QDockWidget*> widgets = findDockWidgets(rx);

        return widgets;
    }

    QDockWidget* getBiggestDockWidget()
    {
        QDockWidget* biggest = NULL;
        uint64_t biggest_area = 0;

        Q_FOREACH(QDockWidget* dock, getAllDockWidgets() )
        {
            const uint64_t area =
                static_cast<uint64_t>(dock->width()) *
                static_cast<uint64_t>(dock->height());

            if(area > biggest_area){
                biggest_area = area;
                biggest = dock;
            }
        }

        return biggest;
    }

    bool canShowChannel(const OmID channelID)
    {
        ChannelDataWrapper cdw(channelID);
        if(cdw.IsChannelValid())
        {
            if(cdw.IsBuilt()){
                return true;

            } else {
                OmTellInfo("channel " + om::string::num(channelID) + " is not built");
                return false;
            }
        }

        return false;
    }

    bool canShowSegmentation(const OmID segmentationID)
    {
        SegmentationDataWrapper sdw(segmentationID);
        if(sdw.IsSegmentationValid())
        {
            if(sdw.IsBuilt()){
                return true;
            } else {
                OmTellInfo("segmentation " + om::string::num(segmentationID) + " is not built");
                return false;
            }
        }

        return false;
    }

    void wireDockWithView2d(QWidget* widget,
                            const std::pair<QDockWidget*,QDockWidget*>& dockAndCompliment)
    {
        OmView2d* w = reinterpret_cast<OmView2d*>(widget);

        QDockWidget* dock = dockAndCompliment.first;
        QDockWidget* complimentaryDock = dockAndCompliment.second;

        connectVisibilityChange(w, dock);
        setComplimentaryDockWidget(w, dock, complimentaryDock);
    }

    void connectVisibilityChange(OmView2d* w, QDockWidget* dock);

    void setComplimentaryDockWidget(OmView2d* w,
                                    QDockWidget* dock,
                                    QDockWidget* complimentaryDock);

    ViewGroupWidgetInfo CreateView3d()
    {
        QString name = "3D View";

        ViewGroupWidgetInfo vgw(name, VIEW3D);

        if(doesDockWidgetExist(makeObjectName(vgw))){
            delete getDockWidget(makeObjectName(vgw));
        }

        vgw.widget = new OmView3d(mainWindow_, vgs_);

        return vgw;
    }

    ViewGroupWidgetInfo CreateView2dChannel(const OmID chanID, const ViewType viewType)
    {
        ChannelDataWrapper cdw(chanID);
        OmChannel& chan = cdw.GetChannel();

        const QString name = getViewName(chan.GetName(), viewType);

        ViewGroupWidgetInfo vgw(name, VIEW2D_CHAN, viewType);

        if(doesDockWidgetExist(makeObjectName(vgw))){
            delete getDockWidget(makeObjectName(vgw));
        }

        vgw.widget = new OmView2d(viewType, mainWindow_, vgs_,
                                  &chan, name.toStdString());

        return vgw;
    }

    ViewGroupWidgetInfo CreateView2dSegmentation(const OmID segmentationID, const ViewType viewType)
    {
        SegmentationDataWrapper sdw(segmentationID);

        const QString name = getViewName(sdw.GetName().toStdString(), viewType);

        ViewGroupWidgetInfo vgw(name, VIEW2D_SEG, viewType);

        if(doesDockWidgetExist(makeObjectName(vgw))){
            delete getDockWidget(makeObjectName(vgw));
        }

        vgw.widget = new OmView2d(viewType, mainWindow_, vgs_,
                                  sdw.GetSegmentationPtr(), name.toStdString());

        return vgw;
    }
};
