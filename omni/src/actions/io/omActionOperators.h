#pragma once

class OmProjectCloseActionImpl;
class OmProjectSaveActionImpl;
class OmSegmentGroupActionImpl;
class OmSegmentJoinActionImpl;
class OmSegmentSelectActionImpl;
class OmSegmentSplitActionImpl;
class OmSegmentUncertainActionImpl;
class OmSegmentValidateActionImpl;
class OmSegmentationThresholdChangeActionImpl;
class OmVoxelSetValueActionImpl;

#include <QDataStream>

QDataStream& operator<<(QDataStream& out, const OmSegmentSplitActionImpl&);
QDataStream& operator>>(QDataStream& in,  OmSegmentSplitActionImpl&);

QDataStream& operator<<(QDataStream& out, const OmSegmentGroupActionImpl&);
QDataStream& operator>>(QDataStream& in,  OmSegmentGroupActionImpl&);

QDataStream& operator<<(QDataStream& out, const OmSegmentJoinActionImpl&);
QDataStream& operator>>(QDataStream& in,  OmSegmentJoinActionImpl&);

QDataStream& operator<<(QDataStream& out, const OmSegmentSelectActionImpl&);
QDataStream& operator>>(QDataStream& in,  OmSegmentSelectActionImpl&);

QDataStream& operator<<(QDataStream& out, const OmSegmentValidateActionImpl&);
QDataStream& operator>>(QDataStream& in,  OmSegmentValidateActionImpl&);

QDataStream& operator<<(QDataStream& out, const OmSegmentUncertainActionImpl&);
QDataStream& operator>>(QDataStream& in, OmSegmentUncertainActionImpl&);

QDataStream& operator<<(QDataStream& out, const OmProjectSaveActionImpl&);
QDataStream& operator>>(QDataStream& in,  OmProjectSaveActionImpl&);

QDataStream& operator<<(QDataStream& out, const OmProjectCloseActionImpl&);
QDataStream& operator>>(QDataStream& in,  OmProjectCloseActionImpl&);

