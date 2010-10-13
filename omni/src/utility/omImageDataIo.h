#ifndef OM_IMAGE_IO_H
#define OM_IMAGE_IO_H

#include "common/omCommon.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataWrapper.h"
#include <QFileInfoList>

class OmImageDataIo {
public:
	static Vector3i om_imagedata_get_dims_hdf5( QFileInfoList,
						    const OmDataPath);
};

#endif
