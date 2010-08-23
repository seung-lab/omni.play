#ifndef OM_IMAGE_IO_H
#define OM_IMAGE_IO_H

#include "common/omCommon.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataWrapper.h"
#include <QFileInfoList>

class vtkImageReader2;
class vtkImageWriter;
class vtkImageData;
class OmDataReader;

class OmImageDataIo {
public:
	static void clearImageData(vtkImageData *);

	static OmDataWrapperPtr allocImageData(Vector3i, OmDataWrapperPtr);
	static OmDataWrapperPtr createBlankImageData(Vector3i, OmDataWrapperPtr,
						     char value = 0);

	static void copyImageData(OmDataWrapperPtr, const DataBbox &,
				  OmDataWrapperPtr, const DataBbox &);

	static void * copyImageData(OmDataWrapperPtr, const DataBbox &);

	static Vector3<int> om_imagedata_get_dims_hdf5( QFileInfoList,
							const OmDataPath);
private:
	static Vector3<int> om_imagedata_get_dims_vtk(QFileInfoList);

	static void getVtkExtentFromAxisAlignedBoundingBox(const AxisAlignedBoundingBox<int>& aabb,
							   int extent[]);
	static void setAxisAlignedBoundingBoxFromVtkExtent(const int extent[],
							   AxisAlignedBoundingBox<int>& aabb);
};

#endif
