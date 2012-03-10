#include "datalayer/archive/old/omMipVolumeArchive.h"
#include "project/omProject.h"

QDataStream& operator<<(QDataStream& out, const OmMipVolCoords& c)
{
    out << c.normToDataMat_;
    out << c.normToDataInvMat_;
    out << c.dataExtent_;
    out << c.dataResolution_;
    out << c.chunkDim_;
    out << c.unitString_;
    out << c.dataStretchValues_;

    out << c.mMipLeafDim;
    out << c.mMipRootLevel;

    return out;
}

QDataStream& operator>>(QDataStream& in, OmMipVolCoords& c)
{
    in >> c.normToDataMat_;
    in >> c.normToDataInvMat_;
    in >> c.dataExtent_;
    in >> c.dataResolution_;
    in >> c.chunkDim_;
    in >> c.unitString_;
    in >> c.dataStretchValues_;

    in >> c.mMipLeafDim;
    in >> c.mMipRootLevel;

    return in;
}

