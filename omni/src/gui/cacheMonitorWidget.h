#ifndef CACHE_MONITOR_WIDGET_H
#define CACHE_MONITOR_WIDGET_H

#include <QtCore/QVariant>
#include <QtGui>
#include <QWidget>

#include "gui/mainwindow.h"

#include "common/omStd.h"


class CacheMonitorWidget : public QWidget {

Q_OBJECT

 public:
	CacheMonitorWidget(QWidget *parent);
	~CacheMonitorWidget();

	QGroupBox* ShowRAMDisplay();
	QGroupBox* ShowVRAMDisplay();
	
 signals:
	void triggerCacheView();

 private:
	MainWindow* mParentWindow;
	long mMaxSize;
	QTimer *mTimer;
	int mCacheIndex;
	int mRAMNumber;
	int mVRAMNumber;

	QGroupBox* groupBox;
	QGridLayout* gridLayout;
	QGroupBox* groupBoxV;
	QGridLayout* gridLayoutV;


	void showInitialView();


};

#endif
