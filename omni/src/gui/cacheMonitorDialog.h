#ifndef CACHE_MONITOR_DIALOG_H
#define CACHE_MONITOR_DIALOG_H

#include <QtGui>

#include "gui/cacheMonitorWidget.h"

class MainWindow;

class CacheMonitorDialog : public QDialog
{
    Q_OBJECT

    public:
    CacheMonitorDialog( QWidget * parent);
    ~CacheMonitorDialog();

private Q_SLOTS:
    void Refresh();
    void done();

private:
    QVBoxLayout* layout;
    QTimer* mTimer;
    CacheMonitorWidget* mCacheMonitorWidget;
    bool mDying;
};

#endif
