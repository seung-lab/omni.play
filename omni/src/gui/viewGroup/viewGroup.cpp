#include "common/omDebug.h"
#include "gui/mainWindow/mainWindow.h"
#include "gui/menubar.h"
#include "gui/viewGroup/viewGroup.h"
#include "project/omProject.h"
#include "utility/dataWrappers.h"
#include "view2d/omView2d.h"
#include "view3d/omView3d.h"
#include "viewGroup/omViewGroupState.h"

static const ViewType UpperLeft  = XY_VIEW;
static const ViewType UpperRight = YZ_VIEW;
static const ViewType LowerLeft  = XZ_VIEW;

ViewGroup::ViewGroup(MainWindow* mw, OmViewGroupState* vgs)
    : ViewGroupUtils(mw, vgs)
    , mainWindow_(mw)
    , vgs_(vgs)
{}

void ViewGroup::AddView3D()
{
    QString name = "3D View";

    ViewGroupWidgetInfo vgw( name, VIEW3D );

    if( doesDockWidgetExist( makeObjectName( vgw ) ) ){
        delete getDockWidget( makeObjectName( vgw ) );
    }

    vgw.widget = new OmView3d( mainWindow_, vgs_ );

    insertDockIntoGroup( vgw );
}

QDockWidget* ViewGroup::AddView2Dchannel(const OmID chanID, const ViewType vtype)
{
    ChannelDataWrapper cdw(chanID);
    OmChannel& chan = cdw.GetChannel();

    const QString name = getViewName(chan.GetName(), vtype);

    ViewGroupWidgetInfo vgw(name, VIEW2D_CHAN, vtype);

    if( doesDockWidgetExist(makeObjectName(vgw))){
        delete getDockWidget(makeObjectName(vgw));
    }

    OmView2d* v2d = new OmView2d(vtype, mainWindow_, vgs_,
                                 &chan, name.toStdString());
    vgw.widget = v2d;

    std::pair<QDockWidget*, QDockWidget*> dockAndCompliment =
        insertDockIntoGroup(vgw);

    wireDockWithView2d(v2d, dockAndCompliment);

    return dockAndCompliment.first;
}

std::pair<QDockWidget*, QDockWidget*>
ViewGroup::AddView2Dsegmentation(const OmID segmentation_id, const ViewType vtype)
{
    SegmentationDataWrapper sdw(segmentation_id);

    const QString name = getViewName(sdw.GetName().toStdString(), vtype);

    ViewGroupWidgetInfo vgw(name, VIEW2D_SEG, vtype);

    if( doesDockWidgetExist(makeObjectName(vgw))){
        delete getDockWidget(makeObjectName(vgw));
    }

    OmView2d* v2d = new OmView2d(vtype, mainWindow_, vgs_,
                                 sdw.GetSegmentationPtr(), name.toStdString());

    vgw.widget = v2d;

    std::pair<QDockWidget*, QDockWidget*> dockAndCompliment =
        insertDockIntoGroup(vgw);

    wireDockWithView2d(v2d, dockAndCompliment);

    return dockAndCompliment;
}

QDockWidget* ViewGroup::makeDockWidget(ViewGroupWidgetInfo& vgw)
{
    QDockWidget* dock = new QDockWidget( vgw.name, mainWindow_);
    vgw.widget->setParent(dock);

    dock->setObjectName( makeObjectName(vgw) );
    dock->setWidget(vgw.widget);
    dock->setAllowedAreas(Qt::AllDockWidgetAreas);
    dock->setAttribute(Qt::WA_DeleteOnClose);

    mainWindow_->mMenuBar->GetWindowMenu()->addAction(dock->toggleViewAction());

    return dock;
}

QDockWidget* ViewGroup::chooseDockToSplit(ViewGroupWidgetInfo& vgw)
{
    QDockWidget* dock = getBiggestDockWidget();
    vgw.dir = Qt::Horizontal;

    if( VIEW2D_CHAN == vgw.widgetType ){
        if( UpperLeft == vgw.vtype ){

        } else if( UpperRight == vgw.vtype ){
            if( doesDockWidgetExist(CHANNEL, UpperLeft)){
                dock = getDockWidget(CHANNEL, UpperLeft);
            }
        } else {
            if( doesDockWidgetExist(CHANNEL, UpperLeft)){
                dock = getDockWidget(CHANNEL, UpperLeft);
            }
            vgw.dir = Qt::Vertical;
        }

    } else if( VIEW2D_SEG == vgw.widgetType ){
        if( UpperLeft == vgw.vtype ){

        } else if( UpperRight == vgw.vtype ){
            if( doesDockWidgetExist(SEGMENTATION, UpperLeft)){
                dock = getDockWidget(SEGMENTATION, UpperLeft);
            }
        } else {
            if( doesDockWidgetExist(SEGMENTATION, UpperLeft)){
                dock = getDockWidget(SEGMENTATION, UpperLeft);
            }
            vgw.dir = Qt::Vertical;
        }

    } else {
        if( doesDockWidgetExist(CHANNEL, UpperRight)){
            dock = getDockWidget(CHANNEL, UpperRight);
        } else {
            if( doesDockWidgetExist(SEGMENTATION, UpperRight)){
                dock = getDockWidget(SEGMENTATION, UpperRight);
            }
        }
        vgw.dir = Qt::Vertical;
    }

    return dock;
}

QDockWidget* ViewGroup::chooseDockToTabify(ViewGroupWidgetInfo& vgw)
{
    if(VIEW3D == vgw.widgetType){
        return NULL;
    }

    QDockWidget* widgetToTabify = NULL;
    const QString complimentaryObjName = makeComplimentaryObjectName(vgw);
    if( doesDockWidgetExist(complimentaryObjName) ){
        widgetToTabify = getDockWidget(complimentaryObjName);
    }

    return widgetToTabify;
}

std::pair<QDockWidget*, QDockWidget*>
ViewGroup::insertDockIntoGroup(ViewGroupWidgetInfo& vgw)
{
    QDockWidget* dock = NULL;
    QDockWidget* dockToTabify = NULL;

    if(0 == getNumDockWidgets())
    {
        dock = makeDockWidget( vgw );
        mainWindow_->addDockWidget(Qt::TopDockWidgetArea, dock);

    } else {
        dockToTabify = chooseDockToTabify(vgw);
        if(dockToTabify){
            dock = insertByTabbing( vgw, dockToTabify );
        } else {
            dock = insertBySplitting(vgw, chooseDockToSplit(vgw));
            QApplication::processEvents();
        }
    }

    return std::make_pair(dock, dockToTabify);
}

QDockWidget* ViewGroup::insertBySplitting(ViewGroupWidgetInfo& vgw, QDockWidget* biggest)
{
    QList<QDockWidget*> tabified = mainWindow_->tabifiedDockWidgets(biggest);
    if(!tabified.empty()){
        Q_FOREACH( QDockWidget* widget, tabified ){
            mainWindow_->removeDockWidget( widget );
        }
    }

    Qt::Orientation dir = vgw.dir;

    QDockWidget* dock = makeDockWidget(vgw);

    //debug(viewGroup, "\t inserting %s by splitting...\n",
    //qPrintable(dock->objectName()));

    mainWindow_->splitDockWidget( biggest, dock, dir );

    if(!tabified.empty())
    {
        Q_FOREACH(QDockWidget* dwidget, tabified )
        {
            dwidget->show();
            mainWindow_->tabifyDockWidget( biggest, dwidget );
        }
    }

    return dock;
}

QDockWidget* ViewGroup::insertByTabbing(ViewGroupWidgetInfo& vgw, QDockWidget* widgetToTabify)
{
    QDockWidget* dock = makeDockWidget( vgw );
    //debug(viewGroup, "\t inserting %s by tabbing...\n",
    //qPrintable(dock->objectName()));
    mainWindow_->tabifyDockWidget( widgetToTabify, dock );

    return dock;
}

void ViewGroup::AddXYView(const OmID channelID, const OmID segmentationID)
{
    Q_FOREACH(QDockWidget* w, getAllDockWidgets() ){
        delete w;
    }

    ChannelDataWrapper cdw(channelID);
    SegmentationDataWrapper sdw(segmentationID);

    const bool validChan = cdw.IsChannelValid() && cdw.IsBuilt();
    const bool validSeg = sdw.IsSegmentationValid() && sdw.IsBuilt();

    if(validChan){
        AddView2Dchannel(channelID, XY_VIEW);
    }

    if(validSeg){
        AddView2Dsegmentation(segmentationID, XY_VIEW);
    }
}

void ViewGroup::AddAllViews(const OmID channelID, const OmID segmentationID)
{
    Q_FOREACH(QDockWidget* w, getAllDockWidgets() ){
        delete w;
    }

    ChannelDataWrapper cdw(channelID);
    SegmentationDataWrapper sdw(segmentationID);

    const bool validChan = cdw.IsChannelValid() && cdw.IsBuilt();
    const bool validSeg = sdw.IsSegmentationValid() && sdw.IsBuilt();

    if( validChan && validSeg){
        AddView2Dchannel(channelID, UpperLeft);
        AddView2Dchannel(channelID, UpperRight);
        AddView2Dchannel(channelID, LowerLeft);

        AddView2Dsegmentation(segmentationID, UpperLeft);
        std::pair<QDockWidget*, QDockWidget*> dockAndCompliment =
            AddView2Dsegmentation(segmentationID, UpperRight);
        AddView2Dsegmentation(segmentationID, LowerLeft);

        AddView3D();

        // raise must be done after adding view3d (Qt timing reasons?)
        dockAndCompliment.second->raise();

    } else {
        if(validChan){
            AddView2Dchannel(channelID, UpperLeft);
            AddView2Dchannel(channelID, UpperRight);
            AddView2Dchannel(channelID, LowerLeft);
        }
        if(validSeg){
            AddView2Dsegmentation(segmentationID, UpperLeft);
            AddView2Dsegmentation(segmentationID, UpperRight);
            AddView2Dsegmentation(segmentationID, LowerLeft);
            AddView3D();
        }
    }
}
