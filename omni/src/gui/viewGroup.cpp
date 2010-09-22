#include "gui/mainwindow.h"
#include "gui/menubar.h"
#include "gui/viewGroup.h"
#include "gui/viewGroupWidgetInfo.h"
#include "project/omProject.h"
#include "system/viewGroup/omViewGroupState.h"
#include "view2d/omView2d.h"
#include "view3d/omView3d.h"
#include "volume/omChannel.h"
#include "volume/omSegmentation.h"
#include "volume/omVolume.h"

static const ViewType UpperLeft  = XY_VIEW;
static const ViewType UpperRight = YZ_VIEW;
static const ViewType LowerLeft  = XZ_VIEW;

ViewGroup::ViewGroup( MainWindow * mw, OmViewGroupState * vgs )
	: mMainWindow(mw)
	, mViewGroupState(vgs)
{
}

int ViewGroup::getID()
{
	return mViewGroupState->GetId();
}

QString ViewGroup::viewGroupName()
{
	return "ViewGroup" + QString::number(getID());
}

QString ViewGroup::makeObjectName()
{
	return "3d_" + viewGroupName();
}

QString ViewGroup::makeObjectName( ObjectType voltype, ViewType vtype )
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

QString ViewGroup::makeObjectName( ViewGroupWidgetInfo * vgw )
{
	if( VIEW2D_CHAN == vgw->widgetType ) {
		return makeObjectName( CHANNEL, vgw->vtype );
	} else if (VIEW2D_SEG  == vgw->widgetType ){
		return makeObjectName( SEGMENTATION, vgw->vtype );
	} else {
		return makeObjectName();
	}
}

QString ViewGroup::makeComplimentaryObjectName( ViewGroupWidgetInfo * vgw )
{
	if( VIEW2D_CHAN == vgw->widgetType ){
		return makeObjectName( SEGMENTATION, vgw->vtype );
	} else if( VIEW2D_SEG == vgw->widgetType ){
		return makeObjectName( CHANNEL, vgw->vtype );
	} else {
		return "";
	}
}

QDockWidget* ViewGroup::getDockWidget( QString objName )
{
	QList<QDockWidget * > widgets = mMainWindow->findChildren< QDockWidget *>( objName );

	if( widgets.isEmpty() ){
		return NULL;
	} else if( widgets.size() > 1 ){
		assert(0);
	}

	return widgets[0];
}

bool ViewGroup::doesDockWidgetExist( QString objName )
{
	QList<QDockWidget * > widgets = mMainWindow->findChildren< QDockWidget *>( objName );

	if( widgets.isEmpty() ){
		return false;
	}

	return true;
}

QList<QDockWidget * > ViewGroup::getAllDockWidgets()
{
	QRegExp rx( ".*" + viewGroupName() + "$" );
	QList<QDockWidget * > widgets = mMainWindow->findChildren< QDockWidget *>( rx );

	return widgets;
}

int ViewGroup::getNumDockWidgets()
{
	QList<QDockWidget * > widgets = getAllDockWidgets();
	return widgets.size();
}

void ViewGroup::addView3D()
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

QWidget * ViewGroup::addView2Dchannel( const OmId chan_id, ViewType vtype)
{
	QString name = getViewName( OmProject::GetChannel(chan_id).GetName(), vtype );
	ViewGroupWidgetInfo * vgw = new ViewGroupWidgetInfo( name, VIEW2D_CHAN, vtype );

	if( doesDockWidgetExist( makeObjectName( vgw ) ) ){
		delete getDockWidget( makeObjectName( vgw ) );
	}

	vgw->widget = new OmView2d(vtype, CHANNEL, chan_id, mMainWindow, mViewGroupState );

	insertDockIntoGroup( vgw );

	QWidget * ret = vgw->widget;

	delete vgw;

	return ret;
}

void ViewGroup::addView2Dsegmentation( const OmId segmentation_id, ViewType vtype)
{
	QString name = getViewName( OmProject::GetSegmentation(segmentation_id).GetName(), vtype );
	ViewGroupWidgetInfo * vgw = new ViewGroupWidgetInfo( name, VIEW2D_SEG, vtype );

	if( doesDockWidgetExist( makeObjectName( vgw ) ) ){
		delete getDockWidget( makeObjectName( vgw ) );
	}

	vgw->widget = new OmView2d(vtype, SEGMENTATION, segmentation_id, mMainWindow, mViewGroupState);

	insertDockIntoGroup( vgw );

	delete vgw;
}

QString ViewGroup::getViewName( const std::string & volName, ViewType vtype )
{
	return QString::fromStdString(volName)
		+ " -- "
		+ getViewTypeAsStr(vtype)
		+ " View";
}

QString ViewGroup::getViewTypeAsStr( ViewType vtype )
{
	if (vtype == XY_VIEW) {
		return "XY";
	} else if (vtype == XZ_VIEW) {
		return "XZ";
	} else {
		return "YZ";
	}
}

QDockWidget * ViewGroup::makeDockWidget( ViewGroupWidgetInfo * vgw )
{
	QDockWidget * dock = new QDockWidget( vgw->name, mMainWindow);
	vgw->widget->setParent(dock);

	dock->setObjectName( makeObjectName(vgw) );
	dock->setWidget(vgw->widget);
	dock->setAllowedAreas(Qt::AllDockWidgetAreas);
	dock->setAttribute(Qt::WA_DeleteOnClose);

	mMainWindow->mMenuBar->getWindowMenu()->addAction(dock->toggleViewAction());

	return dock;
}

QDockWidget * ViewGroup::getBiggestDockWidget()
{
       QDockWidget * biggest = NULL;
       long long biggest_area = 0;

       foreach( QDockWidget* dock, getAllDockWidgets() ){
               long long area = dock->width() * dock->height();
               if( area > biggest_area ){
                       biggest_area = area;
                       biggest = dock;
               }
       }

       return biggest;
}

QDockWidget * ViewGroup::chooseDockToSplit( ViewGroupWidgetInfo * vgw )
{
	QDockWidget * dock = getBiggestDockWidget();
	vgw->dir = Qt::Horizontal;

	if( VIEW2D_CHAN == vgw->widgetType ){
		if( UpperLeft == vgw->vtype ){

		} else if( UpperRight == vgw->vtype ){
			if( doesDockWidgetExist( makeObjectName( CHANNEL, UpperLeft ) ) ){
				dock = getDockWidget( makeObjectName( CHANNEL, UpperLeft ) );
			}
		} else {
			if( doesDockWidgetExist( makeObjectName( CHANNEL, UpperLeft ) ) ){
				dock = getDockWidget( makeObjectName( CHANNEL, UpperLeft ) );
			}
			vgw->dir = Qt::Vertical;
		}
	} else if( VIEW2D_SEG == vgw->widgetType ){
		if( UpperLeft == vgw->vtype ){

		} else if( UpperRight == vgw->vtype ){
			if( doesDockWidgetExist( makeObjectName( SEGMENTATION, UpperLeft ) ) ){
				dock = getDockWidget( makeObjectName( SEGMENTATION, UpperLeft ) );
			}
		} else {
			if( doesDockWidgetExist( makeObjectName( SEGMENTATION, UpperLeft ) ) ){
				dock = getDockWidget( makeObjectName( SEGMENTATION, UpperLeft ) );
			}
			vgw->dir = Qt::Vertical;
		}
	} else {
		if( doesDockWidgetExist( makeObjectName( CHANNEL, UpperRight ) ) ){
			dock = getDockWidget( makeObjectName( CHANNEL, UpperRight ) );
		} else {
			if( doesDockWidgetExist( makeObjectName( SEGMENTATION, UpperRight ) ) ){
				dock = getDockWidget( makeObjectName( SEGMENTATION, UpperRight ) );
			}
		}
		vgw->dir = Qt::Vertical;
	}

	return dock;
}

QDockWidget * ViewGroup::chooseDockToTabify( ViewGroupWidgetInfo * vgw )
{
	if( VIEW3D == vgw->widgetType ){
		return NULL;
	}

	QDockWidget * widgetToTabify = NULL;
	QString complimentaryObjName = makeComplimentaryObjectName( vgw );
	if( doesDockWidgetExist(complimentaryObjName) ){
		widgetToTabify = getDockWidget(complimentaryObjName);
	}

	return widgetToTabify;
}

void ViewGroup::insertDockIntoGroup( ViewGroupWidgetInfo * vgw )
{
	if( 0 == getNumDockWidgets() ) {
		QDockWidget * dock = makeDockWidget( vgw );
		mMainWindow->addDockWidget(Qt::TopDockWidgetArea, dock);
	} else {
		QDockWidget * dockToTabify = chooseDockToTabify( vgw );
		if( NULL != dockToTabify ){
			insertByTabbing( vgw, dockToTabify );
		} else {
			insertBySplitting( vgw, chooseDockToSplit( vgw ));
			QApplication::processEvents();
		}
	}
}

void ViewGroup::insertBySplitting( ViewGroupWidgetInfo * vgw, QDockWidget * biggest )
{
	QList<QDockWidget *> tabified = mMainWindow->tabifiedDockWidgets( biggest );
	if( !tabified.empty() ){
		foreach( QDockWidget* widget, tabified ){
			mMainWindow->removeDockWidget( widget );
		}
	}

	Qt::Orientation dir = vgw->dir;

	QDockWidget * dock = makeDockWidget( vgw );

	debug("viewGroup", "\t inserting %s by splitting...\n", qPrintable(dock->objectName()));
	mMainWindow->splitDockWidget( biggest, dock, dir );

	if( !tabified.empty() ){
		foreach( QDockWidget* dwidget, tabified ){
			dwidget->show();
			mMainWindow->tabifyDockWidget( biggest, dwidget );
		}
	}
}

void ViewGroup::insertByTabbing( ViewGroupWidgetInfo * vgw, QDockWidget * widgetToTabify )
{
	QDockWidget * dock = makeDockWidget( vgw );
	debug("viewGroup", "\t inserting %s by tabbing...\n", qPrintable(dock->objectName()));
	mMainWindow->tabifyDockWidget( widgetToTabify, dock );
}

void ViewGroup::addAllViews(const OmId channelID, const OmId segmentationID )
{
	foreach(QDockWidget * w, getAllDockWidgets() ){
		delete w;
	}

	if( OmProject::IsChannelValid(channelID) ){
		addView2Dchannel( channelID, UpperLeft);
		addView2Dchannel( channelID, UpperRight);
		addView2Dchannel( channelID, LowerLeft);
	}

	if( OmProject::IsSegmentationValid(segmentationID)) {
		addView2Dsegmentation( segmentationID, UpperLeft);
		addView2Dsegmentation( segmentationID, UpperRight);
		addView2Dsegmentation( segmentationID, LowerLeft);
	}

	addView3D();
}
