#include "omDataArchiveSegment.h"
#include "omDataArchiveCoords.h"
#include "omDataArchiveVmml.h"
#include <QDataStream>
#include "segment/omSegment.h"

QDataStream &operator<<(QDataStream & out, const QHash<OmId, OmSegment*> & page )
{
	int numSegs = page.size();
	out << numSegs;

	foreach( OmSegment * segment, page ){
		out << segment->mID;
		out << segment->mValue;
		out << segment->mColor;
		out << segment->mThreshold;
		
		out << segment->segmentsJoinedIntoMe;
		out << segment->parentSegID;
		out << segment->chunks;
	}

	return out;
}

QDataStream &operator>>(QDataStream & in, QHash<OmId, OmSegment*> & page )
{
	int numSegs;
	in >> numSegs;

	for( int i = 0; i < numSegs; ++i ){
		OmSegment * segment = new OmSegment();

		in >> segment->mID;
		in >> segment->mValue;
		in >> segment->mColor;
		in >> segment->mThreshold;
		
		in >> segment->segmentsJoinedIntoMe;
		in >> segment->parentSegID;
		in >> segment->chunks;

		page[ segment->mID ] = segment;
	}

	return in;
}

void OmDataArchiveSegment::ArchiveRead( OmHdf5Path path, QHash<OmId, OmSegment*> & page ) 
{
	int size;
	char* p_data = (char*) OmProjectData::GetProjectDataReader()->dataset_raw_read(path, &size);
	
	QByteArray ba = QByteArray::fromRawData( p_data, size );
	QDataStream in(&ba, QIODevice::ReadOnly);
	in.setByteOrder( QDataStream::LittleEndian );
	in.setVersion(QDataStream::Qt_4_6);
	in >> page;

	delete p_data;
}

void OmDataArchiveSegment::ArchiveWrite( OmHdf5Path path, QHash<OmId, OmSegment*> & page ) 
{
	QByteArray ba;
	QDataStream out(&ba, QIODevice::WriteOnly);
	out.setByteOrder( QDataStream::LittleEndian );
	out.setVersion(QDataStream::Qt_4_6);
	out << page;
	
	OmProjectData::GetDataWriter()->dataset_raw_create_tree_overwrite( path, 
									   ba.size(), 
									   ba.data() );
}
