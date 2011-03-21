#include "gui/cacheMonitorDialog.h"
#include "gui/cacheMonitorWidget.h"
#include "system/cache/omThreadedCache.h"
#include "system/omConnect.hpp"

CacheMonitorDialog::CacheMonitorDialog( QWidget * parent)
    : QDialog(parent)
{
    mTimer = new QTimer(this);
    om::connect(mTimer, SIGNAL(timeout()), this, SLOT(Refresh()));
    mTimer->start(500);

    mCacheMonitorWidget = new CacheMonitorWidget(this);
    layout = new QVBoxLayout(this);
    layout->addWidget(mCacheMonitorWidget);
    mDying = false;
    setWindowTitle(tr("Cache Monitor"));
}

CacheMonitorDialog::~CacheMonitorDialog()
{
    delete mCacheMonitorWidget;
}

void CacheMonitorDialog::Refresh()
{
    if (mDying){
        mTimer->stop();
    } else {
        layout->removeWidget(mCacheMonitorWidget);
        delete mCacheMonitorWidget;
        mCacheMonitorWidget = new CacheMonitorWidget(this);
        layout->addWidget(mCacheMonitorWidget);
        show();
    }
}

void CacheMonitorDialog::done()
{
    mDying = true;
    mTimer->stop();
    delete this;
}
