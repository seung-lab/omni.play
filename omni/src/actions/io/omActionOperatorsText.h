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

#include <QTextStream>

QTextStream& operator<<(QTextStream& out, const OmVoxelSetValueActionImpl&);
QTextStream& operator<<(QTextStream& out, const OmProjectCloseActionImpl&);
QTextStream& operator<<(QTextStream& out, const OmProjectSaveActionImpl&);
QTextStream& operator<<(QTextStream& out, const OmSegmentGroupActionImpl&);
QTextStream& operator<<(QTextStream& out, const OmSegmentJoinActionImpl&);
QTextStream& operator<<(QTextStream& out, const OmSegmentSelectActionImpl&);
QTextStream& operator<<(QTextStream& out, const OmSegmentSplitActionImpl&);
QTextStream& operator<<(QTextStream& out, const OmSegmentUncertainActionImpl&);
QTextStream& operator<<(QTextStream& out, const OmSegmentValidateActionImpl&);
QTextStream& operator<<(QTextStream& out, const OmSegmentationThresholdChangeActionImpl&);

