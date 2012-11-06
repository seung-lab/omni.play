#include "datalayer/archive/old/omMipVolumeArchive.h"
#include "project/omProject.h"

QDataStream& operator>>(QDataStream& in, om::coords::VolumeSystem& c)
{
    Matrix4f dummyMat;
    in >> dummyMat; // normToDataMat_
    in >> dummyMat; // normToDataInvMat_
    AxisAlignedBoundingBox<int> extent;
    in >> extent;
    c.SetDataDimensions(extent.getDimensions());
    Vector3f resolution;
    in >> resolution;
    c.SetResolution(resolution);
    int chunkDim;
    in >> chunkDim; c.SetChunkDimensions(Vector3i(chunkDim));
    QString dummyQString;
    in >> dummyQString; //c.unitString_
    Vector3f dummyVec;
	in >> dummyVec; //c.dataStretchValues_

	int dummyInt;
	in >> dummyInt;
    in >> dummyInt;
    c.UpdateRootLevel();

    return in;
}

