#ifndef OMMEMMAPPEDVOLUME_HPP
#define OMMEMMAPPEDVOLUME_HPP

#include "volume/omMipChunkCoord.h"

#include <QFile>
#include <zi/mutex>

template <typename T, typename VOL>
class OmMemMappedVolume {
public:
	OmMemMappedVolume();
	~OmMemMappedVolume();


	void AllocMemMapFiles(VOL* vol);
	T* getChunkPtr( OmMipChunkCoord & coord);

private:
	VOL* vol_;
	std::vector<QFile*> mFileVec;
	std::vector<uchar*> mFileMapPtr;
	zi::Mutex mutex_;
	bool alreadyAllocFiles;

	int GetBytesPerSample(){ return sizeof(T); }
};

#endif
