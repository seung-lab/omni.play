#include "datalayer/omDataPath.h"
#include "datalayer/omDataPaths.h"
#include "datalayer/omDataReader.h"
#include "datalayer/omDataWrapper.h"
#include "datalayer/omDataWriter.h"
#include "project/omProject.h"
#include "datalayer/omHDF5Volume.hpp"
#include "system/omProjectData.h"

#include <zi/utility>

template <typename T, typename VOL>
OmHDF5Volume<T,VOL>::OmHDF5Volume(VOL* vol)
	: vol_(vol)
{
}

template <typename T, typename VOL>
OmHDF5Volume<T,VOL>::~OmHDF5Volume()
{
}

template <typename T, typename VOL>
void OmHDF5Volume<T,VOL>::load()
{
	zi::Guard g(mutex_);

}

template <typename T, typename VOL>
void OmHDF5Volume<T,VOL>::create(const std::map<int, Vector3i> & levelsAndDims)
{
	zi::Guard g(mutex_);

	const Vector3i chunkdims = vol_->GetChunkDimensions();

	FOR_EACH(it, levelsAndDims){
		const int level = it->first;
		const Vector3i dims = it->second;

		OmDataPath path(vol_->MipLevelInternalDataPath(level));

		OmProjectData::GetDataWriter()->
			allocateChunkedDataset(path,
							    dims,
							    chunkdims,
							    vol_->getVolDataType());
	}

	printf("OmHDF5Volume done allocating data\n");
}

template <typename T, typename VOL>
T* OmHDF5Volume<T,VOL>::getChunkPtr(const OmMipChunkCoord & /*coord*/)
{
	return NULL;
}
