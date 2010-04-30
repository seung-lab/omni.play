#include "gui/cacheMonitorWidget.h"
#include "system/omCacheManager.h"
#include "system/omThreadedCache.h"


CacheMonitorWidget::CacheMonitorWidget(QWidget * parent) : QWidget(parent)
{
	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	mainLayout->addWidget(ShowRAMDisplay());
	mainLayout->addWidget(ShowVRAMDisplay());
        setLayout(mainLayout); 

	mMaxSize = 0;

	mRAMNumber=0;

}

CacheMonitorWidget::~CacheMonitorWidget()
{
	
}



QGroupBox* CacheMonitorWidget::ShowRAMDisplay()
{
	QGroupBox* groupBox = new QGroupBox("RAM",this);
	QGridLayout* gridLayout = new QGridLayout(this);
	groupBox->setLayout( gridLayout );
	long cacheSize[10];
	QString cacheName[10];
	int progress;
	int cacheIndex=0;
	OmCacheBase* cache = OmCacheManager::Instance()->GetCache(RAM_CACHE_GROUP, cacheIndex);
	
	while(cache != NULL){
		cacheSize[cacheIndex] = cache->GetCacheSize();
		cacheName[cacheIndex] = QString(cache->GetCacheName());
		if (cacheSize[cacheIndex] > mMaxSize) mMaxSize = cacheSize[cacheIndex];  
		cacheIndex++;
		cache = OmCacheManager::GetCache(RAM_CACHE_GROUP, cacheIndex); 
		if (cacheIndex==10) cache = NULL;
	}

	QProgressBar* cacheSizeBar;
	for (int j=0; j<cacheIndex; j++){
		QLabel* label = new QLabel(cacheName[j],groupBox);
		gridLayout->addWidget(label, j*2, 0, 1,3);
		cacheSizeBar = new QProgressBar(groupBox);
		cacheSizeBar->setTextVisible(false);
		progress = (int)(100*((float) cacheSize[j])/((float) mMaxSize));
		cacheSizeBar->setValue(progress);
		gridLayout->addWidget(cacheSizeBar, j*2+1, 0, 1, 3);
		label = new QLabel(QString::number(cacheSize[j]),groupBox);
		gridLayout->addWidget(label, j*2+1, 4, 1,1);	
	}
	
	return groupBox;
}

QGroupBox* CacheMonitorWidget::ShowVRAMDisplay()
{
	QGroupBox* groupBox = new QGroupBox("VRAM",this);
	QGridLayout* gridLayout = new QGridLayout(this);
	groupBox->setLayout( gridLayout );
	long cacheSize[10];
	QString cacheName[10];
	int progress;
	int cacheIndex=0;
	OmCacheBase* cache = OmCacheManager::Instance()->GetCache(VRAM_CACHE_GROUP, cacheIndex);
	
	while(cache != NULL){
		cacheSize[cacheIndex] = cache->GetCacheSize();
		cacheName[cacheIndex] = QString(cache->GetCacheName());
		if (cacheSize[cacheIndex] > mMaxSize) mMaxSize = cacheSize[cacheIndex];  
		cacheIndex++;
		cache = OmCacheManager::GetCache(VRAM_CACHE_GROUP, cacheIndex); 
		if (cacheIndex==10) cache = NULL;
	}

	QProgressBar* cacheSizeBar;
	for (int j=0; j<cacheIndex; j++){
		QLabel* label = new QLabel(cacheName[j],groupBox);
		gridLayout->addWidget(label, j*2, 0, 1,3);
		cacheSizeBar = new QProgressBar(groupBox);
		cacheSizeBar->setTextVisible(false);
		progress = (int)(100*((float) cacheSize[j])/((float) mMaxSize));
		cacheSizeBar->setValue(progress);
		gridLayout->addWidget(cacheSizeBar, j*2+1, 0, 1, 3);
		label = new QLabel(QString::number(cacheSize[j]),groupBox);
		gridLayout->addWidget(label, j*2+1, 4, 1,1);	
	}
	
	return groupBox;
}
