#ifndef CACHE_MONITOR_DIALOG_H
#define CACHE_MONITOR_DIALOG_H

#include <QtCore/QVariant>
#include <QtGui>
#include <QWidget>

#include "gui/mainwindow.h"
#include "gui/cacheMonitorWidget.h"
#include "common/omStd.h"

class CacheMonitorDialog : public QDialog 
{

Q_OBJECT

 public:
	CacheMonitorDialog(MainWindow *parent);	
	~CacheMonitorDialog();

 private slots:
	void Refresh();
	void done();

 private:
	MainWindow* mParentWindow;
	QVBoxLayout* layout;
	QTimer *mTimer;
	CacheMonitorWidget* mCacheMonitorWidget;
	bool mDying;
};

#endif
