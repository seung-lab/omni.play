#include "omDataArchiveVmml.h"
#include <QDataStream>

QDataStream &operator<<(QDataStream & out, const Vector3<float> & v )
{
	out << v.array[0];
	out << v.array[1];
	out << v.array[2];
	return out;
}

QDataStream &operator>>(QDataStream & in, Vector3<float> & v )
{
	in >> v.array[0];
	in >> v.array[1];
	in >> v.array[2];
	return in;
}

QDataStream &operator<<(QDataStream & out, const Vector3<int> & v )
{
	out << v.array[0];
	out << v.array[1];
	out << v.array[2];
	return out;
}

QDataStream &operator>>(QDataStream & in, Vector3<int> & v )
{
	in >> v.array[0];
	in >> v.array[1];
	in >> v.array[2];
	return in;
}

QDataStream &operator<<(QDataStream & out, const Matrix4<float> & m )
{
	for( int i = 0; i < 16; ++i ){
		out << m.array[i];
	}
	return out;
}

QDataStream &operator>>(QDataStream & in, Matrix4<float> & m )
{
	for( int i = 0; i < 16; ++i ){
		in >> m.array[i];
	}
	return in;
}

QDataStream &operator<<(QDataStream & out, const DataBbox & d )
{
	vmml::Vector3<int> min = d.getMin();
	vmml::Vector3<int> max = d.getMax();
	bool dirty = d.isDirty();
	bool empty = d.isEmpty();

	out << min;
	out << max;
	out << dirty;
	out << empty;

	return out;
}

QDataStream &operator>>(QDataStream & in, DataBbox & d )
{
	vmml::Vector3<int> min;
	vmml::Vector3<int> max;
	bool dirty;
	bool empty;

	in >> min;
	in >> max;
	in >> dirty;
	in >> empty;

	vmml::AxisAlignedBoundingBox<int> temp(min, max);
	d.setDirty(dirty);
	d.setEmpty(empty);
	d = temp;

	return in;
}
