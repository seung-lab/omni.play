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

	void Load();
	void Create(const std::map<int, Vector3i>&);
	T* GetChunkPtr(const OmMipChunkCoord & coord) const;

	int GetBytesPerSample() const {
		return sizeof(T);
	}

private:
	VOL* vol_;
	zi::Mutex mutex_;

	std::vector<boost::shared_ptr<OmIMemMappedFile<T> > > maps_;
	void resizeMapsVector();

	std::string getFileName(const int level) const;
};

#endif
