#include "gui/cacheMonitorWidget.h"
#include "system/omCacheManager.h"
#include "system/omThreadedCache.h"
#include "system/omCacheInfo.h"

CacheMonitorWidget::CacheMonitorWidget(QWidget * parent) : QWidget(parent)
{
	QVBoxLayout* mainLayout = new QVBoxLayout(this);
        mainLayout->addWidget( ShowDisplay("RAM", RAM_CACHE_GROUP ));
        mainLayout->addWidget(ShowDisplay("VRAM", VRAM_CACHE_GROUP ));
        setLayout(mainLayout); 
}

CacheMonitorWidget::~CacheMonitorWidget()
{
}

QGroupBox* CacheMonitorWidget::ShowDisplay( QString cacheType, OmCacheGroup cacheGroup )
{
	QGroupBox* groupBox = new QGroupBox( cacheType );
	QGridLayout* gridLayout = new QGridLayout( groupBox );

        long maxSize = 0;
	
	QList< OmCacheInfo > infos = OmCacheManager::Instance()->GetCacheInfo(cacheGroup);
	for( int j = 0; j < infos.size(); ++j ){

		OmCacheInfo info = infos.at(j);

                if( info.cacheSize > maxSize){
                        maxSize = info.cacheSize;
		}

		QLabel* label = new QLabel( info.cacheName, groupBox );
		gridLayout->addWidget(label, j*2, 0, 1, 3);

		QProgressBar * cacheSizeBar = new QProgressBar(groupBox);
		cacheSizeBar->setTextVisible(false);

                int progress = (int)(100*((float) info.cacheSize)/((float) maxSize));

		cacheSizeBar->setValue(progress);

		gridLayout->addWidget(cacheSizeBar, j*2+1, 0, 1, 3);

		label = new QLabel(QString::number( info.cacheSize ), groupBox);
		gridLayout->addWidget(label, j*2+1, 4, 1, 1);	
	}
	
	return groupBox;
}


