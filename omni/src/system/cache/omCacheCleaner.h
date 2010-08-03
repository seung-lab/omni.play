#ifndef OM_CACHE_CLEANER_H
#define OM_CACHE_CLEANER_H

#include "system/cache/omCacheManager.h"
#include <QRunnable>
#include <QWaitCondition>

static const int secsToWaitBeforeCleaning = 10;

class OmCacheCleaner : public QRunnable {
public:
	OmCacheCleaner(OmCacheManager* cm)
		: cm_(cm) {}

	void run()
	{
		while(1){
			mCV.wait(


		}
	}

pviate:
	OmCacheManager *const cm_;
	QWaitCondition mCV;
	
};

#endif
