#include "system/omConnect.hpp"
#include "utility/dataWrappers.h"
#include "common/omDebug.h"
#include "gui/mainWindow/mainWindow.h"
#include "gui/menubar.h"
#include "gui/viewGroup.h"
#include "gui/viewGroupWidgetInfo.h"
#include "project/omProject.h"
#include "viewGroup/omViewGroupState.h"
#include "view2d/omView2d.h"
#include "view3d/omView3d.h"

static const ViewType UpperLeft  = XY_VIEW;
static const ViewType UpperRight = YZ_VIEW;
static const ViewType LowerLeft  = XZ_VIEW;

ViewGroup::ViewGroup(MainWindow* mw, OmViewGroupState* vgs)
    : mMainWindow(mw)
    , mViewGroupState(vgs)
{}

int ViewGroup::getID(){
    return mViewGroupState->GetID();
}

QString ViewGroup::makeObjectName(const ObjectType voltype, const ViewType vtype)
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

QString ViewGroup::makeObjectName(ViewGroupWidgetInfo* vgw)
{
    if( VIEW2D_CHAN == vgw->widgetType ) {
        return makeObjectName( CHANNEL, vgw->vtype );
    }

    if (VIEW2D_SEG  == vgw->widgetType ){
        return makeObjectName( SEGMENTATION, vgw->vtype );
    }

    return makeObjectName();
}

QString ViewGroup::makeComplimentaryObjectName(ViewGroupWidgetInfo* vgw)
{
    if( VIEW2D_CHAN == vgw->widgetType ){
        return makeObjectName( SEGMENTATION, vgw->vtype );
    }

    if( VIEW2D_SEG == vgw->widgetType ){
        return makeObjectName( CHANNEL, vgw->vtype );
    }

    return "";
}

QDockWidget* ViewGroup::getDockWidget(const QString& objName)
{
    QList<QDockWidget*> widgets = findDockWidgets(objName);

    if( widgets.isEmpty() ){
        return NULL;
    } else if( widgets.size() > 1 ){
        throw OmArgException("too many widgets");
    }

    return widgets[0];
}

bool ViewGroup::doesDockWidgetExist(const QString& objName )
{
    QList<QDockWidget*> widgets = findDockWidgets(objName);

    if( widgets.isEmpty() ){
        return false;
    }

    return true;
}

QList<QDockWidget*> ViewGroup::getAllDockWidgets()
{
    QRegExp rx( ".*" + viewGroupName() + "$" );
    QList<QDockWidget*> widgets = findDockWidgets(rx);

    return widgets;
}

int ViewGroup::getNumDockWidgets()
{
    QList<QDockWidget*> widgets = getAllDockWidgets();
    return widgets.size();
}

void ViewGroup::AddView3D()
{
    QString name = "3D View";
    ViewGroupWidgetInfo * vgw = new ViewGroupWidgetInfo( name, VIEW3D );

    if( doesDockWidgetExist( makeObjectName( vgw ) ) ){
        delete getDockWidget( makeObjectName( vgw ) );
    }

    vgw->widget = new OmView3d( mMainWindow, mViewGroupState );

    insertDockIntoGroup( vgw );

    delete(vgw);
}

QDockWidget* ViewGroup::AddView2Dchannel(const OmID chan_id, const ViewType vtype)
{
    OmChannel& chan = OmProject::Volumes().Channels().GetChannel(chan_id);
    const QString name = getViewName(chan.GetName(), vtype);

    boost::shared_ptr<ViewGroupWidgetInfo> vgw =
        boost::make_shared<ViewGroupWidgetInfo>(name, VIEW2D_CHAN, vtype);

    if( doesDockWidgetExist(makeObjectName(vgw.get()))){
        delete getDockWidget(makeObjectName(vgw.get()));
    }

    OmView2d* v2d =  new OmView2d(vtype, mMainWindow, mViewGroupState,
                                  &chan,  name.toStdString());
    vgw->widget = v2d;
    std::pair<QDockWidget*,QDockWidget*> dockAndCompliment =
        insertDockIntoGroup(vgw.get());

    View2dDockWidget::WireDockWithView2d(v2d, dockAndCompliment);

    return dockAndCompliment.first;
}

std::pair<QDockWidget*,QDockWidget*>
ViewGroup::AddView2Dsegmentation(const OmID segmentation_id,
                                 const ViewType vtype)
{
    SegmentationDataWrapper sdw(segmentation_id);

    const QString name = getViewName(sdw.GetName().toStdString(), vtype);

    boost::shared_ptr<ViewGroupWidgetInfo> vgw =
        boost::make_shared<ViewGroupWidgetInfo>(name, VIEW2D_SEG, vtype);

    if( doesDockWidgetExist(makeObjectName(vgw.get()))){
        delete getDockWidget(makeObjectName(vgw.get()));
    }

    OmView2d* v2d = new OmView2d(vtype, mMainWindow, mViewGroupState,
                                 sdw.GetSegmentationPtr(), name.toStdString());

    vgw->widget = v2d;

    std::pair<QDockWidget*,QDockWidget*> dockAndCompliment =
        insertDockIntoGroup(vgw.get());

    View2dDockWidget::WireDockWithView2d(v2d, dockAndCompliment);

    return dockAndCompliment;
}

QDockWidget* ViewGroup::makeDockWidget(ViewGroupWidgetInfo* vgw)
{
    QDockWidget* dock = new QDockWidget( vgw->name, mMainWindow);
    vgw->widget->setParent(dock);

    dock->setObjectName( makeObjectName(vgw) );
    dock->setWidget(vgw->widget);
    dock->setAllowedAreas(Qt::AllDockWidgetAreas);
    dock->setAttribute(Qt::WA_DeleteOnClose);

    mMainWindow->mMenuBar->GetWindowMenu()->addAction(dock->toggleViewAction());

    return dock;
}

QDockWidget* ViewGroup::getBiggestDockWidget()
{
    QDockWidget* biggest = NULL;
    long long biggest_area = 0;

    Q_FOREACH(QDockWidget* dock, getAllDockWidgets() ){
        long long area = dock->width() * dock->height();
        if(area > biggest_area){
            biggest_area = area;
            biggest = dock;
        }
    }

    return biggest;
}

QDockWidget* ViewGroup::chooseDockToSplit(ViewGroupWidgetInfo* vgw)
{
    QDockWidget* dock = getBiggestDockWidget();
    vgw->dir = Qt::Horizontal;

    if( VIEW2D_CHAN == vgw->widgetType ){
        if( UpperLeft == vgw->vtype ){

        } else if( UpperRight == vgw->vtype ){
            if( doesDockWidgetExist(CHANNEL, UpperLeft)){
                dock = getDockWidget(CHANNEL, UpperLeft);
            }
        } else {
            if( doesDockWidgetExist(CHANNEL, UpperLeft)){
                dock = getDockWidget(CHANNEL, UpperLeft);
            }
            vgw->dir = Qt::Vertical;
        }
    } else if( VIEW2D_SEG == vgw->widgetType ){
        if( UpperLeft == vgw->vtype ){

        } else if( UpperRight == vgw->vtype ){
            if( doesDockWidgetExist(SEGMENTATION, UpperLeft)){
                dock = getDockWidget(SEGMENTATION, UpperLeft);
            }
        } else {
            if( doesDockWidgetExist(SEGMENTATION, UpperLeft)){
                dock = getDockWidget(SEGMENTATION, UpperLeft);
            }
            vgw->dir = Qt::Vertical;
        }
    } else {
        if( doesDockWidgetExist(CHANNEL, UpperRight)){
            dock = getDockWidget(CHANNEL, UpperRight);
        } else {
            if( doesDockWidgetExist(SEGMENTATION, UpperRight)){
                dock = getDockWidget(SEGMENTATION, UpperRight);
            }
        }
        vgw->dir = Qt::Vertical;
    }

    return dock;
}

QDockWidget* ViewGroup::chooseDockToTabify(ViewGroupWidgetInfo* vgw)
{
    if(VIEW3D == vgw->widgetType){
        return NULL;
    }

    QDockWidget* widgetToTabify = NULL;
    const QString complimentaryObjName = makeComplimentaryObjectName(vgw);
    if( doesDockWidgetExist(complimentaryObjName) ){
        widgetToTabify = getDockWidget(complimentaryObjName);
    }

    return widgetToTabify;
}

std::pair<QDockWidget*,QDockWidget*>
ViewGroup::insertDockIntoGroup(ViewGroupWidgetInfo* vgw)
{
    QDockWidget* dock = NULL;
    QDockWidget* dockToTabify = NULL;

    if(0 == getNumDockWidgets()) {
        dock = makeDockWidget( vgw );
        mMainWindow->addDockWidget(Qt::TopDockWidgetArea, dock);
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

QDockWidget* ViewGroup::insertBySplitting(ViewGroupWidgetInfo* vgw,
                                          QDockWidget* biggest)
{
    QList<QDockWidget*> tabified = mMainWindow->tabifiedDockWidgets(biggest);
    if(!tabified.empty()){
        Q_FOREACH( QDockWidget* widget, tabified ){
            mMainWindow->removeDockWidget( widget );
        }
    }

    Qt::Orientation dir = vgw->dir;

    QDockWidget* dock = makeDockWidget(vgw);

    //debug(viewGroup, "\t inserting %s by splitting...\n",
    //qPrintable(dock->objectName()));

    mMainWindow->splitDockWidget( biggest, dock, dir );

    if(!tabified.empty()){
        Q_FOREACH(QDockWidget* dwidget, tabified ){
            dwidget->show();
            mMainWindow->tabifyDockWidget( biggest, dwidget );
        }
    }

    return dock;
}

QDockWidget* ViewGroup::insertByTabbing(ViewGroupWidgetInfo * vgw,
                                        QDockWidget* widgetToTabify)
{
    QDockWidget* dock = makeDockWidget( vgw );
    //debug(viewGroup, "\t inserting %s by tabbing...\n",
    //qPrintable(dock->objectName()));
    mMainWindow->tabifyDockWidget( widgetToTabify, dock );

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
        std::pair<QDockWidget*,QDockWidget*> dockAndCompliment =
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

QList<QDockWidget*> ViewGroup::findDockWidgets(const QString& name){
    return mMainWindow->findChildren<QDockWidget*>(name);
}

QList<QDockWidget*> ViewGroup::findDockWidgets(const QRegExp& regExp){
    return mMainWindow->findChildren<QDockWidget*>(regExp);
}

void View2dDockWidget::WireDockWithView2d(OmView2d* w,
                                          const std::pair<QDockWidget*,QDockWidget*>& dockAndCompliment)
{
    QDockWidget* dock = dockAndCompliment.first;
    QDockWidget* complimentaryDock = dockAndCompliment.second;

    connectVisibilityChange(w, dock);
    setComplimentaryDockWidget(w, dock, complimentaryDock);
}

void View2dDockWidget::connectVisibilityChange(OmView2d* w, QDockWidget* dock)
{
    // used to let tile cache know when view2d visibilty changes
    om::connect(dock, SIGNAL(visibilityChanged(bool)),
                w, SLOT(dockVisibilityChanged(bool)));
}

void View2dDockWidget::setComplimentaryDockWidget(OmView2d* w,
                                                  QDockWidget* dock,
                                                  QDockWidget* complimentaryDock)
{
    // used for cntrl+tab key in view2d

    if(!complimentaryDock){
        return;
    }

    w->SetComplimentaryDockWidget(complimentaryDock);

    QWidget* compWidget = complimentaryDock->widget();
    if(compWidget){
        OmView2d* v2d = static_cast<OmView2d*>(compWidget);
        v2d->SetComplimentaryDockWidget(dock);
    }
}
