#ifndef CACHE_MONITOR_WIDGET_H
#define CACHE_MONITOR_WIDGET_H

#include "common/omCommon.h"
#include <QtGui>

class CacheMonitorWidget : public QWidget {
	Q_OBJECT

public:
	CacheMonitorWidget(QWidget *parent);

signals:
	void triggerCacheView();

private:
	QGroupBox* showDisplay(const QString&, const OmCacheGroupEnum);
};

#endif
