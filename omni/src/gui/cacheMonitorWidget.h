#ifndef CACHE_MONITOR_WIDGET_H
#define CACHE_MONITOR_WIDGET_H

#include <QtGui>
#include "system/omCacheManager.h"

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
