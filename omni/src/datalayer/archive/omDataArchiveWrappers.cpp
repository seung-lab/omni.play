#include "utility/dataWrappers.h"
#include "datalayer/archive/omDataArchiveWrappers.h"

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
