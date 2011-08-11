#pragma once

class SegmentationDataWrapper;
class SegmentDataWrapper;

QDataStream& operator<<(QDataStream& out, const SegmentationDataWrapper& sdw);
QDataStream& operator>>(QDataStream& in, SegmentationDataWrapper& sdw);

QDataStream& operator<<(QDataStream& out, const SegmentDataWrapper& sdw);
QDataStream& operator>>(QDataStream& in, SegmentDataWrapper& sdw);
