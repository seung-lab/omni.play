#include "utility/dataWrappers.h"
#include "datalayer/archive/old/omDataArchiveWrappers.h"

QDataStream& operator<<(QDataStream& out, const SegmentationDataWrapper& sdw)
{
    out << sdw.GetSegmentationID();
    return out;
}

QDataStream& operator>>(QDataStream& in, SegmentationDataWrapper& sdw)
{
    int id;
    in >> id;
    sdw = SegmentationDataWrapper(id);

    return in;
}

QDataStream& operator<<(QDataStream& out, const SegmentDataWrapper& sdw)
{
    out << sdw.GetSegmentationID();
    out << sdw.GetSegmentID();
    return out;
}

QDataStream& operator>>(QDataStream& in, SegmentDataWrapper& sdw)
{
    uint32_t segmentationID;
    uint32_t segmentID;

    in >> segmentationID;
       in >> segmentID;

    sdw = SegmentDataWrapper(segmentationID, segmentID);

    return in;
}
