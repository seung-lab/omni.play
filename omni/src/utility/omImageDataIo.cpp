#include "common/omDebug.h"
#include "datalayer/omDataLayer.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omIDataReader.h"
#include "datalayer/omDataPaths.h"
#include "datalayer/omDataWrapper.h"
#include "datalayer/hdf5/omHdf5.h"
#include "utility/omImageDataIo.h"

Vector3i OmImageDataIo::om_imagedata_get_dims_hdf5( QFileInfoList sourceFilenamesAndPaths, const OmDataPath dataset )
{
	assert((sourceFilenamesAndPaths.size() == 1) && "More than one hdf5 file specified.h");

	OmIDataReader* hdf5reader =
		OmDataLayer::getReader(sourceFilenamesAndPaths[0].filePath(),
				       true );

	hdf5reader->open();

	//get dims of image
	Vector3i dims;
	if(hdf5reader->dataset_exists(dataset)){
		dims = hdf5reader->getChunkedDatasetDims( dataset );
	} else {
		dims = hdf5reader->getChunkedDatasetDims( OmDataPaths::getDefaultDatasetName() );
	}

	debug("hfd5image", "dims are %i,%i,%i\n", DEBUGV3(dims));

	hdf5reader->close();

	return dims;
}
