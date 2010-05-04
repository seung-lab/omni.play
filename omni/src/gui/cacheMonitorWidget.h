#ifndef CACHE_MONITOR_WIDGET_H
#define CACHE_MONITOR_WIDGET_H

#include <QtCore/QVariant>
#include <QtGui>
#include <QWidget>

#include "gui/mainwindow.h"

#include "common/omStd.h"


class CacheMonitorWidget : public QWidget 
{

Q_OBJECT

 public:
	CacheMonitorWidget(QWidget *parent);
	~CacheMonitorWidget();
	
 signals:
	void triggerCacheView();

 private:
	QGroupBox* ShowDisplay( QString cacheType, OmCacheGroup cacheGroup );
};

#endif
