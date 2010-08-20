#ifndef OMMEMMAPPEDVOLUME_HPP
#define OMMEMMAPPEDVOLUME_HPP

#include "volume/omMipChunkCoord.h"

#include <QFile>
#include <zi/mutex>

template <typename T, typename VOL>
class OmMemMappedVolume {
public:
	OmMemMappedVolume(){} // for boost::varient
	OmMemMappedVolume(VOL* vol);

	~OmMemMappedVolume();

	void load();
	void create(const std::map<int, Vector3i> &);

	T* getChunkPtr(const OmMipChunkCoord & coord);
	int GetBytesPerSample(){ return sizeof(T); }

private:
	VOL* vol_;
	std::vector<QFile*> mFileVec;
	std::vector<uchar*> mFileMapPtr;
	zi::Mutex mutex_;

	QString getFileName(const int level);
	QFile* openFile(const int level);
	void memMap(QFile * file, const int level);
	void allocateSpace(QFile * file);
	void openAndmMemMap(const int level);
};

#endif
