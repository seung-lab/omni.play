#ifndef OM_MEM_MAPPED_VOLUME_HPP
#define OM_MEM_MAPPED_VOLUME_HPP

#include "datalayer/omIDataVolume.hpp"

#include <zi/mutex>
#include <QFile>

template <typename T> class OmIMemMappedFile;

template <typename T, typename VOL>
class OmMemMappedVolume : public OmIDataVolume<T,VOL> {
public:
	OmMemMappedVolume(){} // for boost::varient
	OmMemMappedVolume(VOL* vol);

	virtual ~OmMemMappedVolume();

	void Load();
	void Create(const std::map<int, Vector3i> &);
	T* GetChunkPtr(const OmMipChunkCoord & coord);

	int GetBytesPerSample(){
		return sizeof(T);
	}

private:
	VOL* vol_;
	zi::Mutex mutex_;

	std::vector<QFile*> mFileVec;
	std::vector<uchar*> mFileMapPtr;

	QString getFileName(const int level);
	QFile* openFile(const int level);
	void memMap(QFile * file, const int level);
	void allocateSpace(QFile * file);
	void openAndmMemMap(const int level);
};

#endif
