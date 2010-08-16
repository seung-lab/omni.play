#ifndef OMMEMMAPPEDVOLUME_HPP
#define OMMEMMAPPEDVOLUME_HPP

#include "volume/omMipChunkCoord.h"

#include <QFile>
#include <zi/mutex>

template <typename T, typename VOL>
class OmMemMappedVolume {
public:
	OmMemMappedVolume(VOL* vol);
	~OmMemMappedVolume();


	void AllocMemMapFiles();
	T* getChunkPtr( OmMipChunkCoord & coord);

private:
	VOL *const vol_;
	std::vector<QFile*> mFileVec;
	std::vector<uchar*> mFileMapPtr;
	std::set<int> openedLevels;
	zi::Mutex mutex_;
	bool alreadyAllocFiles;

	int GetBytesPerSample(){ return sizeof(T); }

	QString getFileName(const int level);
	QFile* openFile(const int level);
	void memMap(QFile * file, const int level);
	void allocateSpace(QFile * file);
	void openAndmMemMap(const int level);

};

#endif
