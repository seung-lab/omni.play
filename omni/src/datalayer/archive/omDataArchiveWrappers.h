#ifndef OM_DATA_ARCHIVE_WRAPPERS_H
#define OM_DATA_ARCHIVE_WRAPPERS_H

class SegmentationDataWrapper;

QDataStream& operator<<(QDataStream& out, const SegmentationDataWrapper& sdw);
QDataStream& operator>>(QDataStream& in, SegmentationDataWrapper& sdw);

#endif
