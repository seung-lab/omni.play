#include "viewGroup.h"
#include "volume/omVolume.h"
#include "view2d/omView2d.h"
#include "view3d/omView3d.h"
#include "gui/mainwindow.h"

ViewGroup::ViewGroup( MainWindow * mainWindow )
	: mMainWindow( mainWindow )
{
}

void ViewGroup::addView3D()
{
	QString name = "3D View";
	OmView3d * view3d = new OmView3d( this );

	QDockWidget * dock = makeDockWidget( view3d, name );
	mMainWindow->addDockWidget(Qt::TopDockWidgetArea, dock);

	dock3D = dock;
}

void ViewGroup::addView2Dchannel( OmId chan_id, ViewType vtype)
{
	QString name = getViewName( OmVolume::GetChannel(chan_id).GetName(), vtype );
	OmView2d * view2d = new OmView2d(vtype, CHANNEL, chan_id, this );

	QDockWidget * dock = makeDockWidget( view2d, name );
	mMainWindow->addDockWidget(Qt::TopDockWidgetArea, dock);

	/*
	if( channelDockWidgets.isEmpty() ){
		mMainWindow->addDockWidget(Qt::TopDockWidgetArea, dock);
	} else {
		if( channelDockWidgets.size() == 1 ){
		}
	}
	*/

	channelDockWidgets[ vtype ] = dock;
}

void ViewGroup::addView2Dsegmentation( OmId segmentation_id, ViewType vtype)
{
	QString name = getViewName( OmVolume::GetSegmentation(segmentation_id).GetName(), vtype );
	OmView2d * view2d = new OmView2d(vtype, SEGMENTATION, segmentation_id, this);

	QDockWidget * dock = makeDockWidget( view2d, name );
	mMainWindow->addDockWidget(Qt::TopDockWidgetArea, dock);
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
	QDockWidget *dock = new QDockWidget(name, mMainWindow);
	dock->setAllowedAreas(Qt::AllDockWidgetAreas);
	dock->setWidget(widget);
	mMainWindow->windowMenu->addAction(dock->toggleViewAction());
	dock->setAttribute(Qt::WA_DeleteOnClose);

	return dock;
}
