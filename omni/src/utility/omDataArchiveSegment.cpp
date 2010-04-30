#include "omDataArchiveSegment.h"
#include "omDataArchiveCoords.h"
#include "omDataArchiveVmml.h"
#include <QDataStream>
#include "segment/omSegment.h"

QDataStream &operator<<(QDataStream & out, const OmSegment & segment )
{
	out << segment.mID;
	out << segment.mValue;
	out << segment.mColor;
	out << segment.mThreshold;

	out << segment.segmentsJoinedIntoMe;
	out << segment.parentSegID;
	out << segment.chunks;

	return out;
}

QDataStream &operator>>(QDataStream & in, OmSegment & segment )
{
	in >> segment.mID;
	in >> segment.mValue;
	in >> segment.mColor;
	in >> segment.mThreshold;

	in >> segment.segmentsJoinedIntoMe;
	in >> segment.parentSegID;
	in >> segment.chunks;

	return in;
}

void OmDataArchiveSegment::ArchiveRead( OmHdf5Path path, OmSegment * segment ) 
{
	int size;
	char* p_data = (char*) OmProjectData::GetProjectDataReader()->dataset_raw_read(path, &size);
	
	QByteArray ba = QByteArray::fromRawData( p_data, size );
	QDataStream in(&ba, QIODevice::ReadOnly);
	in.setByteOrder( QDataStream::LittleEndian );
	in.setVersion(QDataStream::Qt_4_6);
	in >> (*segment);

	delete p_data;
}

void OmDataArchiveSegment::ArchiveWrite( OmHdf5Path path, OmSegment * segment ) 
{
	QByteArray ba;
	QDataStream out(&ba, QIODevice::WriteOnly);
	out.setByteOrder( QDataStream::LittleEndian );
	out.setVersion(QDataStream::Qt_4_6);
	out << (*segment);
	
	OmProjectData::GetDataWriter()->dataset_raw_create_tree_overwrite( path, 
									   ba.size(), 
									   ba.data() );
}
