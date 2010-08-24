#ifndef OM_HDF5_VOLUME_HPP
#define OM_HDF5_VOLUME_HPP

#include "datalayer/omDataVolumeBase.hpp"

#include <zi/mutex>
#include <QFile>

template <typename T, typename VOL>
class OmHDF5Volume : public OmDataVolumeBase<T,VOL> {
public:
	OmHDF5Volume(){} // for boost::varient
	OmHDF5Volume(VOL* vol);

	~OmHDF5Volume();

	void load();
	void create(const std::map<int, Vector3i> &);
	T* getChunkPtr(const OmMipChunkCoord & coord);

	int GetBytesPerSample(){ return sizeof(T); }

private:
	VOL* vol_;
	zi::Mutex mutex_;
};

#endif
