#include "utility/stringHelpers.h"
#include "gui/cacheMonitorWidget.h"
#include "system/cache/omCacheInfo.h"
#include "system/cache/omCacheManager.h"

CacheMonitorWidget::CacheMonitorWidget(QWidget* parent)
	: QWidget(parent)
{
	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	mainLayout->addWidget(showDisplay("RAM", RAM_CACHE_GROUP ));
	mainLayout->addWidget(showDisplay("VRAM", VRAM_CACHE_GROUP ));
	setLayout(mainLayout);
}

QGroupBox* CacheMonitorWidget::showDisplay(const QString& cacheType,
										   const OmCacheGroupEnum cacheGroup )
{
	QGroupBox* groupBox = new QGroupBox( cacheType );
	QGridLayout* gridLayout = new QGridLayout( groupBox );

	long maxSize = 0;

	QList<OmCacheInfo> infos = OmCacheManager::GetCacheInfo(cacheGroup);
	for( int j = 0; j < infos.size(); ++j ){

		OmCacheInfo info = infos.at(j);

		if( info.cacheSize > maxSize){
			maxSize = info.cacheSize;
		}

		QLabel* label = new QLabel( info.cacheName, groupBox );
		gridLayout->addWidget(label, j*2, 0, 1, 3);

		QProgressBar* cacheSizeBar = new QProgressBar(groupBox);
		cacheSizeBar->setTextVisible(false);

		const int progress =
			(int)(100*((float) info.cacheSize)/((float) maxSize));

		cacheSizeBar->setValue(progress);

		gridLayout->addWidget(cacheSizeBar, j*2+1, 0, 1, 3);

		label = new QLabel(StringHelpers::commaDeliminateNumQT(info.cacheSize),
						   groupBox);
		gridLayout->addWidget(label, j*2+1, 4, 1, 1);
	}

	return groupBox;
}


