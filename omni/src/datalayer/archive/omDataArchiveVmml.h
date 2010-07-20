#ifndef OM_DATA_ARCHIVE_VMML_H
#define OM_DATA_ARCHIVE_VMML_H

#include "common/omDebug.h"

QDataStream &operator<<(QDataStream & out, const Vector3<float> & v );
QDataStream &operator>>(QDataStream & in, Vector3<float> & v );

QDataStream &operator<<(QDataStream & out, const Vector3<int> & v );
QDataStream &operator>>(QDataStream & in, Vector3<int> & v );

QDataStream &operator<<(QDataStream & out, const Matrix4<float> & v );
QDataStream &operator>>(QDataStream & in, Matrix4<float> & v );

QDataStream &operator<<(QDataStream & out, const Matrix4<float> & v );
QDataStream &operator>>(QDataStream & in, Matrix4<float> & v );

QDataStream &operator<<(QDataStream & out, const DataBbox & d );
QDataStream &operator>>(QDataStream & in, DataBbox & d );

#endif
