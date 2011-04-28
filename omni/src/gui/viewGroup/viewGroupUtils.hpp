#ifndef VIEW_GROUP_UTILS_HPP
#define VIEW_GROUP_UTILS_HPP

#include "common/omCommon.h"
#include "gui/viewGroup/viewGroupWidgetInfo.h"

#include <QtGui>

class MainWindow;
class OmViewGroupState;
class OmView2d;

class ViewGroupUtils {
private:
    MainWindow *const mainWindow_;
    OmViewGroupState *const vgs_;

public:
    ViewGroupUtils(MainWindow* mainWindow, OmViewGroupState* vgs)
        : mainWindow_(mainWindow)
        , vgs_(vgs)
    {}

    virtual ~ViewGroupUtils()
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

    QString getViewName(const std::string& volName, const ViewType vtype)
    {
        return QString::fromStdString(volName)
            + " -- "
            + getViewTypeAsStr(vtype)
            + " View";
    }

    QString getViewTypeAsStr(const ViewType vtype)
    {
        switch(vtype){
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

    QString makeObjectName(const ObjectType voltype, const ViewType vtype)
    {
        QString name;

        if( CHANNEL == voltype ){
            name = "channel_";
        } else if ( SEGMENTATION == voltype ) {
            name = "segmentation_";
        }

        name += getViewTypeAsStr(vtype) + "_" + viewGroupName();

        return name;
    }

    QString makeObjectName(ViewGroupWidgetInfo& vgw)
    {
        if( VIEW2D_CHAN == vgw.widgetType ) {
            return makeObjectName( CHANNEL, vgw.vtype );
        }

        if (VIEW2D_SEG  == vgw.widgetType ){
            return makeObjectName( SEGMENTATION, vgw.vtype );
        }

        return makeObjectName();
    }

    QString makeComplimentaryObjectName(ViewGroupWidgetInfo& vgw)
    {
        if( VIEW2D_CHAN == vgw.widgetType ){
            return makeObjectName( SEGMENTATION, vgw.vtype );
        }

        if( VIEW2D_SEG == vgw.widgetType ){
            return makeObjectName( CHANNEL, vgw.vtype );
        }

        return "";
    }

    QDockWidget* getDockWidget(const ObjectType voltype, const ViewType vtype){
        return getDockWidget(makeObjectName(voltype, vtype));
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

    bool doesDockWidgetExist(const ObjectType voltype, const ViewType vtype){
        return doesDockWidgetExist(makeObjectName(voltype, vtype));
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

    int getNumDockWidgets()
    {
        QList<QDockWidget*> widgets = getAllDockWidgets();
        return widgets.size();
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

    void wireDockWithView2d(OmView2d* w,
                            const std::pair<QDockWidget*,QDockWidget*>& dockAndCompliment)
    {
        QDockWidget* dock = dockAndCompliment.first;
        QDockWidget* complimentaryDock = dockAndCompliment.second;

        connectVisibilityChange(w, dock);
        setComplimentaryDockWidget(w, dock, complimentaryDock);
    }

    void connectVisibilityChange(OmView2d* w, QDockWidget* dock);

    void setComplimentaryDockWidget(OmView2d* w,
                                    QDockWidget* dock,
                                    QDockWidget* complimentaryDock);
};

#endif
