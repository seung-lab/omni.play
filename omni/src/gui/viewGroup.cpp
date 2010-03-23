#include "viewGroup.h"
#include "volume/omVolume.h"
#include "view2d/omView2d.h"
#include "view3d/omView3d.h"
#include "gui/mainwindow.h"

ViewGroup::ViewGroup( MainWindow * mainWindow )
	: mMainWindow( mainWindow )
{
	dock3D = NULL;
}

void ViewGroup::addView3D()
{
	if( dock3D != NULL ){
		return;
	}

	QString name = "3D View";
	OmView3d * view3d = new OmView3d( this );

	QDockWidget * dock = makeDockWidget( view3d, name );

	insertDockIntoGroup( dock, NULL );
	dock3D = dock;
}

void ViewGroup::addView2Dchannel( OmId chan_id, ViewType vtype)
{
	if( channelDockWidgets.contains( vtype ) ){
		return;
	}

	QString name = getViewName( OmVolume::GetChannel(chan_id).GetName(), vtype );
	OmView2d * view2d = new OmView2d(vtype, CHANNEL, chan_id, this );

	QDockWidget * dock = makeDockWidget( view2d, name );

	QDockWidget * widgetToTabify = NULL;
	if( segmentationDockWidgets.contains( vtype ) ){
		widgetToTabify = segmentationDockWidgets.value( vtype );
	}
	insertDockIntoGroup( dock, widgetToTabify );

	channelDockWidgets[ vtype ] = dock;
}

void ViewGroup::addView2Dsegmentation( OmId segmentation_id, ViewType vtype)
{
	if( segmentationDockWidgets.contains( vtype ) ){
		return;
	}

	QString name = getViewName( OmVolume::GetSegmentation(segmentation_id).GetName(), vtype );
	OmView2d * view2d = new OmView2d(vtype, SEGMENTATION, segmentation_id, this);

	QDockWidget * dock = makeDockWidget( view2d, name );

	QDockWidget * widgetToTabify = NULL;
	if( channelDockWidgets.contains( vtype ) ){
		widgetToTabify = channelDockWidgets.value( vtype );
	}
	insertDockIntoGroup( dock, widgetToTabify );

	segmentationDockWidgets[ vtype ] = dock;
}

QString ViewGroup::getViewName( string baseName, ViewType vtype )
{
	QString name = QString::fromStdString( baseName );

	if (vtype == XY_VIEW) {
		name += " -- XY View";
	} else if (vtype == XZ_VIEW) {
		name += " -- XZ View";
	} else {
		name += " -- YZ View";
	}

	return name;
}

QDockWidget * ViewGroup::makeDockWidget( QWidget * widget, QString name )
{
	QDockWidget * dock = new QDockWidget(name, mMainWindow);
	
	widget->setParent(dock);

	dock->setWidget(widget);
	dock->setAllowedAreas(Qt::AllDockWidgetAreas);
	dock->setAttribute(Qt::WA_DeleteOnClose);

	dock->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
	widget->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

	mMainWindow->windowMenu->addAction(dock->toggleViewAction());

	return dock;
}

int ViewGroup::getNumDockWidgets()
{
	return allDockWidgets.size();
}

QDockWidget * ViewGroup::getBiggestDockWidget()
{
	QDockWidget * biggest = NULL;
	long long biggest_area = 0;

	foreach( QDockWidget* dock, allDockWidgets ){
		long long area = dock->width() * dock->height();
		if( area > biggest_area ){
			biggest_area = area;
			biggest = dock;
		}
	}

	return biggest;
}

void ViewGroup::insertDockIntoGroup( QDockWidget * dock, QDockWidget * widgetToTabify )
{
	QDockWidget * biggest = getBiggestDockWidget();
		
	if( 0 == getNumDockWidgets() || NULL == biggest ){
		mMainWindow->addDockWidget(Qt::TopDockWidgetArea, dock);
	} else {
		if( NULL == widgetToTabify ) {
			insertBySplitting( dock, biggest );
		} else {
			insertByTabbing( dock, widgetToTabify );
		}
	}

	allDockWidgets << dock;
}

void ViewGroup::insertBySplitting( QDockWidget * dock, QDockWidget * biggest )
{
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
			insertByTabbing( widget, biggest );
		}
	}

	QApplication::processEvents();
	biggest->widget()->resize( desiredW, desiredH );
	dock->widget()->resize( desiredW, desiredH );
	biggest->resize( desiredW, desiredH );
	dock->resize( desiredW, desiredH );

	QApplication::processEvents();

	printf("widget size is: %d x %d\n", 
	       dock->widget()->width(), 
	       dock->widget()->height() );
	printf("widget dock size is: %d x %d\n", 
	       dock->width(), 
	       dock->height() );

	printf("biggest widget size is: %d x %d\n", 
	       biggest->widget()->width(), 
	       biggest->widget()->height() );
	printf("biggest widget size is: %d x %d\n", 
	       biggest->width(), 
	       biggest->height() );

}

void ViewGroup::insertByTabbing( QDockWidget * dock, QDockWidget * widgetToTabify )
{
	mMainWindow->tabifyDockWidget( widgetToTabify, dock );
}
