#include "datalayer/archive/old/utilsOld.hpp"
#include "datalayer/archive/old/omMipVolumeArchive.h"
#include "project/omProject.h"

QDataStream& operator<<(QDataStream& out, const OmMipVolCoords& c)
{
    //out << c.normToDataMat_;
    //out << c.normToDataInvMat_;
    out << c.dataDimensions_;
    //out << c.dataResolution_;
    out << c.chunkDim_;

    out << c.mMipLeafDim;
    out << c.mMipRootLevel;

    return out;
}

QDataStream& operator>>(QDataStream& in, OmMipVolCoords& c)
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
    in >> c.chunkDim_;
    QString dummyQString;
    in >> dummyQString; //c.unitString_
    Vector3f dummyVec;
	in >> dummyVec; //c.dataStretchValues_

	in >> c.mMipLeafDim;
    in >> c.mMipRootLevel;

    return in;
}

