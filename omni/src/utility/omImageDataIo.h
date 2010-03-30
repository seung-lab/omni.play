#ifndef OM_IMAGE_IO_H
#define OM_IMAGE_IO_H

/*
 *
 *
 */

#include "volume/omVolumeTypes.h"
#include "common/omStd.h"

#include <vmmlib/vmmlib.h>
#include <vmmlib/serialization.h>
using namespace vmml;

#include <QFileInfoList>

enum ImageType { TIFF_TYPE, JPEG_TYPE, PNG_TYPE, VTK_TYPE, HDF5_TYPE, NONE_TYPE };

class vtkImageReader2;
class vtkImageWriter;
class vtkImageData;

class OmImageDataIo {
 public:
	static bool are_file_names_valid( QFileInfoList sourceFilenamesAndPaths );

	//type
	static ImageType om_imagedata_parse_image_type(QString fileNameAndPath);

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

	static Vector3<int> om_imagedata_get_dims(QFileInfoList sourceFilenamesAndPaths);

	static vtkImageData * om_imagedata_read( QFileInfoList sourceFilenamesAndPaths, 
						 const DataBbox srcExtentBbox, 
						 const DataBbox dataExtentBbox, 
						 int bytesPerSample);
 private:
	
	//vtk io
	static vtkImageReader2 * om_imagedata_get_reader(ImageType);
	static vtkImageReader2 * om_imagedata_get_reader(QString fname);

	//reading
	static vtkImageData * om_imagedata_read_vtk(QFileInfoList sourceFilenamesAndPaths, const DataBbox srcExtentBbox, 
						    const DataBbox dataExtentBbox, int bytesPerSample);
	static vtkImageData * om_imagedata_read_hdf5(QFileInfoList sourceFilenamesAndPaths, const DataBbox srcExtentBbox, 
						     const DataBbox dataExtentBbox, int bytesPerSample);

	//determine dimensions
	static Vector3<int> om_imagedata_get_dims_vtk(QFileInfoList sourceFilenamesAndPaths);
	static Vector3<int> om_imagedata_get_dims_hdf5( QFileInfoList sourceFilenamesAndPaths );

	/////////////////////////////////
	///////		 vtkImageData Utility Functions
	static void getVtkExtentFromAxisAlignedBoundingBox(const AxisAlignedBoundingBox<int>& aabb, int extent[]);
	static void setAxisAlignedBoundingBoxFromVtkExtent(const int extent[], AxisAlignedBoundingBox<int>& aabb);

	static void appendImageDataPairs(vtkImageData **inputImageData, vtkImageData **outputImageData, int num_pairs, int axis);
};

#endif
