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

	insertWidget( view3d, name );
}

void ViewGroup::addView2Dchannel( OmId chan_id, ViewType vtype)
{
	QString name = getViewName( OmVolume::GetChannel(chan_id).GetName(), vtype );
	OmView2d * view2d = new OmView2d(vtype, CHANNEL, chan_id, this );

	insertWidget( view2d, name );
}

void ViewGroup::addView2Dsegmentation( OmId segmentation_id, ViewType vtype)
{
	QString name = getViewName( OmVolume::GetSegmentation(segmentation_id).GetName(), vtype );
	OmView2d * view2d = new OmView2d(vtype, SEGMENTATION, segmentation_id, this);

	insertWidget( view2d, name );
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

void ViewGroup::insertWidget( QWidget * widget, QString name )
{
	QDockWidget *dock = new QDockWidget(name, mMainWindow);
	dock->setAllowedAreas(Qt::AllDockWidgetAreas);

	widget->setParent(dock);
	dock->setWidget(widget);

	mMainWindow->addDockWidget(Qt::TopDockWidgetArea, dock);
	mMainWindow->windowMenu->addAction(dock->toggleViewAction());

	dock->setAttribute(Qt::WA_DeleteOnClose);
}
