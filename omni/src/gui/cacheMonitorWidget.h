#ifndef CACHE_MONITOR_WIDGET_H
#define CACHE_MONITOR_WIDGET_H

#include "common/omCommon.h"
#include <QtGui>

class CacheMonitorWidget : public QWidget {
    Q_OBJECT

public:
    CacheMonitorWidget(QWidget *parent);

Q_SIGNALS:
    void triggerCacheView();

private:
    QGroupBox* showDisplay(const QString&, const om::CacheGroup);
};

#endif
