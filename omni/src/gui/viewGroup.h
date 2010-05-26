#ifndef VIEW_GROUP_H
#define VIEW_GROUP_H

#include <QtGui>
#include <QMap>
#include <string>

#include "gui/viewGroupWidgetInfo.h"
#include "system/viewGroup/omViewGroupState.h"

class MainWindow;
class OmView2d;
class OmView3d;

class ViewGroup : public QWidget
{
 public:
	ViewGroup( MainWindow * mainWindow, OmViewGroupState * viewGroupState );
	void addView2Dchannel( OmId chan_id, ViewType vtype);
	void addView2Dsegmentation( OmId segmentation_id, ViewType vtype);
	void addView3D();
	void addAllViews( OmId channelID, OmId segmentationID );

 private:
	MainWindow * mMainWindow;
	OmViewGroupState * mViewGroupState;
	int getID();
 
	QString getViewName( QString baseName, ViewType vtype );
	QDockWidget *makeDockWidget( ViewGroupWidgetInfo * vgw );
	int getNumDockWidgets();

	void insertDockIntoGroup( ViewGroupWidgetInfo * vgw );
	void insertBySplitting( ViewGroupWidgetInfo * vgw, QDockWidget * biggest );
	void insertByTabbing( ViewGroupWidgetInfo * vgw, QDockWidget * widgetToTabify);
	QString getViewTypeAsStr( ViewType vtype );

	QString viewGroupName();
	QString makeObjectName();
	QString makeObjectName( ObjectType voltype, ViewType vtype );

	bool doesDockWidgetExist( QString objName );
	QList<QDockWidget * > getAllDockWidgets();
	QDockWidget* getDockWidget( QString objName );

	QDockWidget * getBiggestDockWidget();
	QDockWidget * chooseDockToSplit( ViewGroupWidgetInfo * vgw );
	QDockWidget * chooseDockToTabify( ViewGroupWidgetInfo * vgw );

	QString makeObjectName( ViewGroupWidgetInfo * vgw );
	QString makeComplimentaryObjectName( ViewGroupWidgetInfo * vgw );
};

#endif
