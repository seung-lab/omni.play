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
#include <QString>
#include <list>
using std::list;

enum ImageType { TIFF_TYPE, JPEG_TYPE, PNG_TYPE, VTK_TYPE, HDF5_TYPE, NONE_TYPE };

class vtkImageReader2;
class vtkImageWriter;
class vtkImageData;

//type
ImageType om_imagedata_parse_image_type(QString fileNameAndPath);
	
//vtk io
vtkImageReader2* om_imagedata_get_reader(ImageType);
vtkImageReader2* om_imagedata_get_reader(QString fname);
vtkImageWriter* om_imagedata_get_writer(ImageType);
vtkImageWriter* om_imagedata_get_writer(const string &fname);

//reading
vtkImageData* om_imagedata_read( QFileInfoList sourceFilenamesAndPaths, const DataBbox srcExtentBbox, 
				const DataBbox dataExtentBbox, int bytesPerSample);
vtkImageData* om_imagedata_read_vtk(QFileInfoList sourceFilenamesAndPaths, const DataBbox srcExtentBbox, 
				    const DataBbox dataExtentBbox, int bytesPerSample);
vtkImageData* om_imagedata_read_hdf5(QFileInfoList sourceFilenamesAndPaths, const DataBbox srcExtentBbox, 
				     const DataBbox dataExtentBbox, int bytesPerSample);

//determine dimensions
Vector3<int> om_imagedata_get_dims(QFileInfoList sourceFilenamesAndPaths);
Vector3<int> om_imagedata_get_dims_vtk(QFileInfoList sourceFilenamesAndPaths);
Vector3<int> om_imagedata_get_dims_hdf5( QFileInfoList sourceFilenamesAndPaths );

/////////////////////////////////
///////		 vtkImageData Utility Functions
void getVtkExtentFromAxisAlignedBoundingBox(const AxisAlignedBoundingBox<int>& aabb, int extent[]);
void setAxisAlignedBoundingBoxFromVtkExtent(const int extent[], AxisAlignedBoundingBox<int>& aabb);

void printImageData(vtkImageData *data);
void* copyImageData(vtkImageData *srcData, const DataBbox &srcCopyBbox);
void copyImageData(vtkImageData *dstData, const DataBbox &dstCopyBbox,
		   vtkImageData *srcData, const DataBbox &srcCopyBbox);
void copyIntersectedImageDataFromOffset(vtkImageData *dstData, 
					vtkImageData *srcData, const Vector3<int> &srcOffset);

void clearImageData(vtkImageData *data);

vtkImageData* allocImageData(Vector3<int> dims, int bytesPerSample);
vtkImageData* createBlankImageData(Vector3<int> dims, int bytesPerSample, char value = 0);

void appendImageDataPairs(vtkImageData **inputImageData, vtkImageData **outputImageData, int num_pairs, int axis);

#endif
