#ifndef OM_IMAGE_IO_H
#define OM_IMAGE_IO_H

/*
 *
 *
 */

#include "common/omCommon.h"
#include "datalayer/omDataPath.h"
#include <QFileInfoList>



class vtkImageReader2;
class vtkImageWriter;
class vtkImageData;

class OmImageDataIo {
 public:
	static void clearImageData(vtkImageData *data);

	static vtkImageData * allocImageData(Vector3<int> dims, int bytesPerSample);
	static vtkImageData * createBlankImageData(Vector3<int> dims, 
						   int bytesPerSample, 
						   char value = 0);

	static void copyImageData(vtkImageData *dstData, const DataBbox &dstCopyBbox,
				  vtkImageData *srcData, const DataBbox &srcCopyBbox);

	static void * copyImageData(vtkImageData *srcData, const DataBbox &srcCopyBbox);

	static void copyIntersectedImageDataFromOffset(vtkImageData *dstData, 
						       vtkImageData *srcData, 
						       const Vector3<int> &srcOffset);
	static Vector3<int> om_imagedata_get_dims_hdf5( QFileInfoList sourceFilenamesAndPaths, const OmDataPath dataset);

	static vtkImageData * om_imagedata_read_hdf5(QFileInfoList sourceFilenamesAndPaths, 
						     const DataBbox dataExtentBbox, 
						     int bytesPerSample, const OmDataPath dataset);
 private:
	
	//reading
	static vtkImageData * om_imagedata_read_vtk(QFileInfoList sourceFilenamesAndPaths, 
						    const DataBbox srcExtentBbox, 
						    const DataBbox dataExtentBbox, 
						    int bytesPerSample);
	
	// vtkImageData Utility Functions
	static void getVtkExtentFromAxisAlignedBoundingBox(const AxisAlignedBoundingBox<int>& aabb, int extent[]);
	static void setAxisAlignedBoundingBoxFromVtkExtent(const int extent[], AxisAlignedBoundingBox<int>& aabb);

	static void appendImageDataPairs(vtkImageData **inputImageData, vtkImageData **outputImageData, int num_pairs, int axis);
};

#endif
