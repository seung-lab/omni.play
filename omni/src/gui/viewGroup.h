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

class ViewGroup : public QWidget
{
 public:
	ViewGroup( MainWindow * mainWindow );
	void addView2Dchannel( OmId chan_id, ViewType vtype);
	void addView2Dsegmentation( OmId segmentation_id, ViewType vtype);
	void addView3D();

 private:
	MainWindow * mMainWindow;

	QDockWidget * dock3D;
	QMap<ViewType, QDockWidget*> channelDockWidgets;
	QMap<ViewType, QDockWidget*> segmentationDockWidgets;
 
	QString getViewName( std::string baseName, ViewType vtype );
	QDockWidget *makeDockWidget( QWidget * widget, QString name );
};

#endif
