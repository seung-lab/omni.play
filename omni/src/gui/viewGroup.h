#ifndef VIEW_GROUP_H
#define VIEW_GROUP_H

#include "common/omCommon.h"

#include <QtGui>

class ViewGroupWidgetInfo;
class OmViewGroupState;
class MainWindow;
class OmView2d;
class OmView3d;

class View2dDockWidget{
public:
    static void WireDockWithView2d(OmView2d*,
                                   const std::pair<QDockWidget*,QDockWidget*>&);
private:
    static void connectVisibilityChange(OmView2d*, QDockWidget*);
    static void setComplimentaryDockWidget(OmView2d* ,
                                           QDockWidget*, QDockWidget*);
};

class ViewGroup{
public:
    ViewGroup(MainWindow*, OmViewGroupState*);

    QDockWidget* AddView2Dchannel(const OmID chan_id,
                                  const ViewType vtype);

    std::pair<QDockWidget*,QDockWidget*>
    AddView2Dsegmentation(const OmID segmentation_id, const ViewType vtype);

    void AddView3D();
    void AddAllViews(const OmID channelID, const OmID segmentationID);
    void AddXYView(const OmID channelID, const OmID segmentationID);

private:
    MainWindow * mMainWindow;
    OmViewGroupState * mViewGroupState;
    int getID();

    QDockWidget *makeDockWidget(ViewGroupWidgetInfo*);
    int getNumDockWidgets();

    std::pair<QDockWidget*,QDockWidget*> insertDockIntoGroup(ViewGroupWidgetInfo*);
    QDockWidget* insertBySplitting(ViewGroupWidgetInfo*, QDockWidget* );
    QDockWidget* insertByTabbing(ViewGroupWidgetInfo*, QDockWidget*);

    QString viewGroupName(){
        return "ViewGroup" + QString::number(getID());
    }
    QString makeObjectName(){
        return "3d_" + viewGroupName();
    }
    QString makeObjectName(const ObjectType, const ViewType);

    bool doesDockWidgetExist(const QString&);
    bool doesDockWidgetExist(const ObjectType voltype, const ViewType vtype){
        return doesDockWidgetExist(makeObjectName(voltype, vtype));
    }

    QList<QDockWidget*> getAllDockWidgets();

    QDockWidget* getDockWidget(const QString&);
    QDockWidget* getDockWidget(const ObjectType voltype, const ViewType vtype){
        return getDockWidget(makeObjectName(voltype, vtype));
    }

    QDockWidget* getBiggestDockWidget();
    QDockWidget* chooseDockToSplit(ViewGroupWidgetInfo*);
    QDockWidget* chooseDockToTabify(ViewGroupWidgetInfo*);

    QString makeObjectName(ViewGroupWidgetInfo*);
    QString makeComplimentaryObjectName(ViewGroupWidgetInfo*);

    QList<QDockWidget*> findDockWidgets(const QString&);
    QList<QDockWidget*> findDockWidgets(const QRegExp&);

    QString getViewName(const std::string& volName, const ViewType vtype){
        return QString::fromStdString(volName)
            + " -- "
            + getViewTypeAsStr(vtype)
            + " View";
    }

    QString getViewTypeAsStr(const ViewType vtype){
        if (vtype == XY_VIEW) {
            return "XY";
        } else if (vtype == XZ_VIEW) {
            return "XZ";
        } else {
            return "YZ";
        }
    }
};

#endif
