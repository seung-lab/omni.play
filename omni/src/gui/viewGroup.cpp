#include "viewGroup.h"
#include "volume/omVolume.h"
#include "view2d/omView2d.h"
#include "view3d/omView3d.h"
#include "gui/mainwindow.h"

ViewGroup::ViewGroup( MainWindow * mainWindow )
	: mMainWindow( mainWindow ), mID(1)
{
}

QString ViewGroup::viewGroupName()
{
	return "ViewGroup" + QString::number(mID);
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

QDockWidget* ViewGroup::getDockWidget( QString objName )
{
	debug("viewGroup", "in %s...\n", __FUNCTION__ );

	QList<QDockWidget * > widgets = mMainWindow->findChildren< QDockWidget *>( objName );

	if( widgets.isEmpty() ){
		return NULL;
	} else if( widgets.size() > 1 ){
		assert(0);
	}
	
	return widgets[0];
}

bool ViewGroup::doesDockWidgetexist( QString objName )
{
	debug("viewGroup", "in %s...\n", __FUNCTION__ );

	QList<QDockWidget * > widgets = mMainWindow->findChildren< QDockWidget *>( objName );

	if( widgets.isEmpty() ){
		return false;
	}
	
	return true;
}

QList<QDockWidget * > ViewGroup::getAllDockWidgets()
{
	debug("viewGroup", "in %s...\n", __FUNCTION__ );

	QRegExp rx( ".*" + viewGroupName() + "$" );
	QList<QDockWidget * > widgets = mMainWindow->findChildren< QDockWidget *>( rx );

	foreach( QDockWidget * w, widgets ){
		debug("viewGroup", "found %s\n", qPrintable(w->objectName()));
	}

	return widgets;
}

int ViewGroup::getNumDockWidgets()
{
	debug("viewGroup", "in %s...\n", __FUNCTION__ );

	QList<QDockWidget * > widgets = getAllDockWidgets();
	return widgets.size();
}

void ViewGroup::addView3D()
{
	debug("viewGroup", "in %s...\n", __FUNCTION__ );

	QString objName = makeObjectName();

	if( doesDockWidgetexist( objName ) ){
		return;
	}

	QString name = "3D View";
	OmView3d * view3d = new OmView3d( this );

	ViewGroupWidget * vgw = new ViewGroupWidget( view3d, name, objName );

	insertDockIntoGroup( vgw, NULL );

	delete(vgw);
}

void ViewGroup::addView2Dchannel( OmId chan_id, ViewType vtype)
{
	debug("viewGroup", "in %s...\n", __FUNCTION__ );

	QString objName = makeObjectName( CHANNEL, vtype );

	if( doesDockWidgetexist( objName ) ){
		return;
	}
	
	QString name = getViewName( OmVolume::GetChannel(chan_id).GetName(), vtype );
	OmView2d * view2d = new OmView2d(vtype, CHANNEL, chan_id, this );

	ViewGroupWidget * vgw = new ViewGroupWidget( view2d, name, objName );
	
	QDockWidget * widgetToTabify = NULL;
	QString complimentaryObjName = makeObjectName( SEGMENTATION, vtype );
	if( doesDockWidgetexist(complimentaryObjName) ){
		widgetToTabify = getDockWidget(complimentaryObjName);
	}

	insertDockIntoGroup( vgw, widgetToTabify );

	delete(vgw);
}

void ViewGroup::addView2Dsegmentation( OmId segmentation_id, ViewType vtype)
{
	debug("viewGroup", "in %s...\n", __FUNCTION__ );

	QString objName = makeObjectName( SEGMENTATION, vtype );

	if( doesDockWidgetexist( objName ) ){
		return;
	}

	QString name = getViewName( OmVolume::GetSegmentation(segmentation_id).GetName(), vtype );
	OmView2d * view2d = new OmView2d(vtype, SEGMENTATION, segmentation_id, this);

	ViewGroupWidget * vgw = new ViewGroupWidget( view2d, name, objName );

	QDockWidget * widgetToTabify = NULL;
	QString complimentaryObjName = makeObjectName( CHANNEL, vtype );
	if( doesDockWidgetexist(complimentaryObjName) ){
		widgetToTabify = getDockWidget(complimentaryObjName);
	}

	insertDockIntoGroup( vgw, widgetToTabify );

	delete(vgw);
}

QString ViewGroup::getViewName( string baseName, ViewType vtype )
{
	debug("viewGroup", "in %s...\n", __FUNCTION__ );

	QString name = QString::fromStdString( baseName );
	name += " -- " + getViewTypeAsStr(vtype) + " View";
	return name;
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

QDockWidget * ViewGroup::makeDockWidget( ViewGroupWidget * vgw )
{
	debug("viewGroup", "in %s...\n", __FUNCTION__ );

	QDockWidget * dock = new QDockWidget( vgw->name, mMainWindow);
	vgw->widget->setParent(dock);

	dock->setObjectName( vgw->objName );
	dock->setWidget(vgw->widget);
	dock->setAllowedAreas(Qt::AllDockWidgetAreas);
	dock->setAttribute(Qt::WA_DeleteOnClose);

	dock->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
	vgw->widget->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

	mMainWindow->windowMenu->addAction(dock->toggleViewAction());

	return dock;
}

QDockWidget * ViewGroup::getBiggestDockWidget()
{
	debug("viewGroup", "in %s...\n", __FUNCTION__ );

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

void ViewGroup::insertDockIntoGroup( ViewGroupWidget * vgw, QDockWidget * widgetToTabify )
{
	debug("viewGroup", "in %s...\n", __FUNCTION__ );

	QDockWidget * biggest = getBiggestDockWidget();
		
	if( 0 == getNumDockWidgets() || NULL == biggest ){
		QDockWidget * dock = makeDockWidget( vgw );
		mMainWindow->addDockWidget(Qt::TopDockWidgetArea, dock);
	} else {
		if( NULL == widgetToTabify ) {
			insertBySplitting( vgw, biggest );
		} else {
			insertByTabbing( vgw, widgetToTabify );
		}
	}
}

void ViewGroup::insertBySplitting( ViewGroupWidget * vgw, QDockWidget * biggest )
{
	debug("viewGroup", "in %s...\n", __FUNCTION__ );

	const int h = biggest->widget()->height();
	const int w = biggest->widget()->width();

	QList<QDockWidget *> tabified = mMainWindow->tabifiedDockWidgets( biggest );
	if( !tabified.empty() ){
		foreach( QDockWidget* widget, tabified ){
			mMainWindow->removeDockWidget( widget );
		}
	}

	int desiredW = w;
	int desiredH = h;

	QDockWidget * dock = makeDockWidget( vgw );

	if( w > h ){
		mMainWindow->splitDockWidget( biggest, dock, Qt::Horizontal );
		desiredW = w / 2;
	} else {
		mMainWindow->splitDockWidget( biggest, dock, Qt::Vertical );
		desiredH = h / 2;
	}

	if( !tabified.empty() ){
		foreach( QDockWidget* widget, tabified ){
			widget->show();
			mMainWindow->tabifyDockWidget( biggest, widget );
		}
	}

	/*
	QApplication::processEvents();
	biggest->widget()->resize( desiredW, desiredH );
	dock->widget()->resize( desiredW, desiredH );
	QApplication::processEvents();
	biggest->resize( desiredW, desiredH );
	dock->resize( desiredW, desiredH );
	QApplication::processEvents();
	*/

	debug("viewGroup", "widget size is: %d x %d\n", 
	       dock->widget()->width(), 
	       dock->widget()->height() );
	debug("viewGroup", "widget dock size is: %d x %d\n", 
	       dock->width(), 
	       dock->height() );

	debug("viewGroup", "biggest widget size is: %d x %d\n", 
	       biggest->widget()->width(), 
	       biggest->widget()->height() );
	debug("viewGroup", "biggest widget size is: %d x %d\n", 
	       biggest->width(), 
	       biggest->height() );

}

void ViewGroup::insertByTabbing( ViewGroupWidget * vgw, QDockWidget * widgetToTabify )
{
	debug("viewGroup", "in %s...\n", __FUNCTION__ );

	QDockWidget * dock = makeDockWidget( vgw );
	mMainWindow->tabifyDockWidget( widgetToTabify, dock );
}
