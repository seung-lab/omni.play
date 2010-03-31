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

	foreach( QDockWidget * w, widgets ){
		debug("viewGroup", "found %s\n", qPrintable(w->objectName()));
	}

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
		delete( getDockWidget( makeObjectName( vgw ) ) );
	}

	vgw->widget = new OmView3d( this );

	insertDockIntoGroup( vgw );

	delete(vgw);
}

void ViewGroup::addView2Dchannel( OmId chan_id, ViewType vtype)
{
	QString name = getViewName( OmVolume::GetChannel(chan_id).GetName(), vtype );
	ViewGroupWidgetInfo * vgw = new ViewGroupWidgetInfo( name, VIEW2D_CHAN, vtype );

	if( doesDockWidgetExist( makeObjectName( vgw ) ) ){
		delete( getDockWidget( makeObjectName( vgw ) ) );
	}
	
	vgw->widget = new OmView2d(vtype, CHANNEL, chan_id, this );

	insertDockIntoGroup( vgw );

	delete(vgw);
}

void ViewGroup::addView2Dsegmentation( OmId segmentation_id, ViewType vtype)
{
	QString name = getViewName( OmVolume::GetSegmentation(segmentation_id).GetName(), vtype );
	ViewGroupWidgetInfo * vgw = new ViewGroupWidgetInfo( name, VIEW2D_SEG, vtype );

	if( doesDockWidgetExist( makeObjectName( vgw ) ) ){
		delete( getDockWidget( makeObjectName( vgw ) ) );
	}

	vgw->widget = new OmView2d(vtype, SEGMENTATION, segmentation_id, this);

	insertDockIntoGroup( vgw );

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

QDockWidget * ViewGroup::makeDockWidget( ViewGroupWidgetInfo * vgw )
{
	debug("viewGroup", "in %s...\n", __FUNCTION__ );

	QDockWidget * dock = new QDockWidget( vgw->name, mMainWindow);
	vgw->widget->setParent(dock);

	dock->setObjectName( makeObjectName(vgw) );
	dock->setWidget(vgw->widget);
	dock->setAllowedAreas(Qt::AllDockWidgetAreas);
	dock->setAttribute(Qt::WA_DeleteOnClose);

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

QDockWidget * ViewGroup::chooseDockToSplit( ViewGroupWidgetInfo * vgw )
{
	QDockWidget * dock = getBiggestDockWidget();
	vgw->dir = Qt::Horizontal;

	if( VIEW2D_CHAN == vgw->widgetType ){
		if( XY_VIEW == vgw->vtype ){
			vgw->dir = Qt::Horizontal;
		} else if( XZ_VIEW == vgw->vtype ){
			if( doesDockWidgetExist( makeObjectName( CHANNEL, XY_VIEW ) ) ){
				dock = getDockWidget( makeObjectName( CHANNEL, XY_VIEW ) );
			}
			vgw->dir = Qt::Horizontal;
		} else {
			if( doesDockWidgetExist( makeObjectName( CHANNEL, XY_VIEW ) ) ){
				dock = getDockWidget( makeObjectName( CHANNEL, XY_VIEW ) );
			}
			vgw->dir = Qt::Vertical;
		}
	} else if( VIEW2D_SEG == vgw->widgetType ){
		if( XY_VIEW == vgw->vtype ){
			vgw->dir = Qt::Horizontal;
		} else if( XZ_VIEW == vgw->vtype ){		
			if( doesDockWidgetExist( makeObjectName( SEGMENTATION, XY_VIEW ) ) ){
				dock = getDockWidget( makeObjectName( SEGMENTATION, XY_VIEW ) );
			}
			vgw->dir = Qt::Horizontal;
		} else {
			if( doesDockWidgetExist( makeObjectName( SEGMENTATION, XY_VIEW ) ) ){
				dock = getDockWidget( makeObjectName( SEGMENTATION, XY_VIEW ) );
			}
			vgw->dir = Qt::Vertical;
		}
	} else {
		if( doesDockWidgetExist( makeObjectName( CHANNEL, XZ_VIEW ) ) ){
			dock = getDockWidget( makeObjectName( CHANNEL, XZ_VIEW ) );
		} else {
			if( doesDockWidgetExist( makeObjectName( SEGMENTATION, XZ_VIEW ) ) ){
				dock = getDockWidget( makeObjectName( SEGMENTATION, XZ_VIEW ) );
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

void ViewGroup::addAllViews(OmId channelID, OmId segmentationID )
{
	foreach(QDockWidget * w, getAllDockWidgets() ){
		delete(w);
	}

	if( OmVolume::IsChannelValid(channelID) ){
		addView2Dchannel( channelID, XY_VIEW);
		addView2Dchannel( channelID, XZ_VIEW);
		addView2Dchannel( channelID, YZ_VIEW);
	}

	if( OmVolume::IsSegmentationValid(segmentationID)) {
		addView2Dsegmentation( segmentationID, XY_VIEW);
		addView2Dsegmentation( segmentationID, XZ_VIEW);
		addView2Dsegmentation( segmentationID, YZ_VIEW);
	}

	addView3D();
}
