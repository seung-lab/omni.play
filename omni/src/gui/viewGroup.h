#ifndef VIEW_GROUP_H
#define VIEW_GROUP_H

#include <QtGui>
#include <QWidget>
#include <QString>
#include <QMap>
#include <string>

#include "system/omSystemTypes.h"

class MainWindow;
class OmView2d;
class OmView3d;

class ViewGroupWidget
{
 public:
	ViewGroupWidget( QWidget* w, QString n, QString on )
		: widget(w), name(n), objName(on) {}
	QWidget * widget;
	QString name;
	QString objName;
};

class ViewGroup : public QWidget
{
 public:
	ViewGroup( MainWindow * mainWindow );
	void addView2Dchannel( OmId chan_id, ViewType vtype);
	void addView2Dsegmentation( OmId segmentation_id, ViewType vtype);
	void addView3D();

 private:
	MainWindow * mMainWindow;
	const int mID;
 
	QString getViewName( std::string baseName, ViewType vtype );
	QDockWidget *makeDockWidget( ViewGroupWidget * vgw );
	QDockWidget * getBiggestDockWidget();
	int getNumDockWidgets();

	void insertDockIntoGroup( ViewGroupWidget * vgw, QDockWidget * widgetToTabify );
	void insertBySplitting( ViewGroupWidget * vgw, QDockWidget * biggest );
	void insertByTabbing( ViewGroupWidget * vgw, QDockWidget * widgetToTabify);
	QString getViewTypeAsStr( ViewType vtype );

	QString viewGroupName();
	QString makeObjectName();
	QString makeObjectName( ObjectType voltype, ViewType vtype );

	bool doesDockWidgetexist( QString objName );
	QList<QDockWidget * > getAllDockWidgets();
	QDockWidget* getDockWidget( QString objName );
};

#endif
