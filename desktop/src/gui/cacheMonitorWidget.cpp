#include "utility/omStringHelpers.h"
#include "gui/cacheMonitorWidget.h"
#include "system/cache/omCacheInfo.h"
#include "system/cache/omCacheManager.h"

CacheMonitorWidget::CacheMonitorWidget(QWidget* parent)
    : QWidget(parent)
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(showDisplay("MESH", om::common::MESH_CACHE));
    mainLayout->addWidget(showDisplay("TILE", om::common::TILE_CACHE));
    setLayout(mainLayout);
}

QGroupBox* CacheMonitorWidget::showDisplay(const QString& cacheType,
                                           const om::common::CacheGroup cacheGroup)
{
    QGroupBox* groupBox = new QGroupBox(cacheType);
    QGridLayout* gridLayout = new QGridLayout(groupBox);

    uint64_t maxSize = 0;

    QList<OmCacheInfo> infos = OmCacheManager::GetCacheInfo(cacheGroup);
    for(int j = 0; j < infos.size(); ++j){

        OmCacheInfo info = infos.at(j);

        if(info.size > maxSize){
            maxSize = info.size;
        }

        QLabel* label = new QLabel(QString::fromStdString(info.name), groupBox);
        gridLayout->addWidget(label, j*2, 0, 1, 3);

        QProgressBar* cacheSizeBar = new QProgressBar(groupBox);
        cacheSizeBar->setTextVisible(false);

        const int progress =
            (int)(100*((float) info.size)/((float) maxSize));

        cacheSizeBar->setValue(progress);

        gridLayout->addWidget(cacheSizeBar, j*2+1, 0, 1, 3);

        label = new QLabel(OmStringHelpers::HumanizeNumQT(info.size),
                           groupBox);
        gridLayout->addWidget(label, j*2+1, 4, 1, 1);
    }

    return groupBox;
}


