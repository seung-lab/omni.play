#ifndef OM_VOLUME_IMPORTER_HPP
#define OM_VOLUME_IMPORTER_HPP

#include "common/omCommon.h"
#include "volume/omVolumeTypes.hpp"

class OmDataPath;
class OmDataReader;
template <typename VOL> class OmVolumeImporterHDF5;

template <typename VOL>
class OmVolumeImporter {
public:
	OmVolumeImporter(VOL*);

	bool import(OmDataPath & dataset);

private:
	VOL *const vol_;
	boost::shared_ptr<OmVolumeImporterHDF5<VOL> > hdf5_;

	bool doImport(OmDataPath& path);
	bool importImageStack();

	bool areImportFilesImages();
	OmVolDataType figureOutDataType(OmDataPath& path);
	OmVolDataType figureOutDataTypeImage();

	void allocateData(const OmVolDataType);
	void allocateMemMap(const std::map<int, Vector3i> &);
};

#endif
