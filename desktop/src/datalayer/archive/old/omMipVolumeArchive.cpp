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
    Vector3i dims;
    in >> dims;
    c.SetDataDimensions(dims);
    Vector3i resolution;
    in >> resolution;
    c.SetResolution(resolution);
    in >> c.chunkDim_;
    QString dummy;
    in >> dummy; //c.dataStretchValues_

    in >> c.mMipLeafDim;
    in >> c.mMipRootLevel;

    return in;
}

