#include "system/cache/omCacheManager.h"
#include "system/cache/omCacheManagerImpl.hpp"

OmCacheManager::OmCacheManager()
{}

void OmCacheManager::Delete(){
	instance().impl_.reset();
}

void OmCacheManager::Reset(){
	instance().impl_ = boost::shared_ptr<OmCacheManagerImpl>(new OmCacheManagerImpl());
}

void OmCacheManager::UpdateCacheSizeFromLocalPrefs(){
	instance().impl_->UpdateCacheSizeFromLocalPrefs();
}

void OmCacheManager::AddCache(const OmCacheGroupEnum group, OmCacheBase* base){
	instance().impl_->AddCache(group, base);
}

void OmCacheManager::RemoveCache(const OmCacheGroupEnum group, OmCacheBase* base){
	instance().impl_->RemoveCache(group, base);
}

QList<OmCacheInfo> OmCacheManager::GetCacheInfo(const OmCacheGroupEnum group){
	return instance().impl_->GetCacheInfo(group);
}

void OmCacheManager::SignalCachesToCloseDown(){
	instance().impl_->SignalCachesToCloseDown();
}

void OmCacheManager::TouchFresheness(){
	instance().impl_->TouchFresheness();
}

uint64_t OmCacheManager::GetFreshness(){
	return instance().impl_->GetFreshness();
}

bool OmCacheManager::AmClosingDown(){
	return instance().impl_->AmClosingDown();
}
