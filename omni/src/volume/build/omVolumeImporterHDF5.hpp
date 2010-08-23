#ifndef OM_VOLUME_IMPORTER_HDF5_HPP
#define OM_VOLUME_IMPORTER_HDF5_HPP

#include "common/omCommon.h"
#include "volume/omVolumeTypes.hpp"

class OmDataPath;
class OmDataReader;

template <typename VOL>
class OmVolumeImporterHDF5 {
public:
	OmVolumeImporterHDF5(VOL*);

	bool importHDF5(OmDataPath & inpath);
	OmVolDataType figureOutDataTypeHDF5(OmDataPath&);
	void allocateHDF5(const std::map<int, Vector3i> &);

private:
	VOL *const vol_;

	OmDataPath getHDFsrcPath(OmDataReader*, const OmDataPath&);
	QString getHDFfileNameAndPath();
};

#endif
