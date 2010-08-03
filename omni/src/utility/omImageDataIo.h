#ifndef OM_IMAGE_IO_H
#define OM_IMAGE_IO_H

#include "common/omCommon.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataWrapper.h"
#include <QFileInfoList>

class vtkImageReader2;
class vtkImageWriter;
class vtkImageData;

class OmImageDataIo {
 public:
	static void clearImageData(vtkImageData *data);

	static OmDataWrapperPtr allocImageData(Vector3<int> dims, OmDataWrapperPtr);
	static OmDataWrapperPtr createBlankImageData(Vector3<int> dims, OmDataWrapperPtr old, char value = 0);

	static void copyImageData(OmDataWrapperPtr dstData, const DataBbox &dstCopyBbox,
				  OmDataWrapperPtr srcData, const DataBbox &srcCopyBbox);

	static void * copyImageData(OmDataWrapperPtr srcData, const DataBbox &srcCopyBbox);

	static void copyIntersectedImageDataFromOffset(OmDataWrapperPtr dstData,
						       OmDataWrapperPtr srcData,
						       const Vector3<int> &srcOffset);
	static Vector3<int> om_imagedata_get_dims_hdf5( QFileInfoList sourceFilenamesAndPaths, const OmDataPath dataset);

	static OmDataWrapperPtr om_imagedata_read_hdf5(QFileInfoList sourceFilenamesAndPaths,
						     const DataBbox dataExtentBbox,
						     const OmDataPath dataset);
 private:

	//determine dimensions
	static Vector3<int> om_imagedata_get_dims_vtk(QFileInfoList sourceFilenamesAndPaths);

	static void getVtkExtentFromAxisAlignedBoundingBox(const AxisAlignedBoundingBox<int>& aabb, int extent[]);
	static void setAxisAlignedBoundingBoxFromVtkExtent(const int extent[],
							   AxisAlignedBoundingBox<int>& aabb);

	static void appendImageDataPairs(vtkImageData **inputImageData,
					 vtkImageData **outputImageData,
					 int num_pairs, int axis);
};

#endif
