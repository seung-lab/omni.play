#include "omDataArchiveSegment.h"
#include "omDataArchiveCoords.h"
#include "omDataArchiveVmml.h"
#include <QDataStream>
#include "segment/omSegment.h"

void OmDataArchiveSegment::ArchiveRead( const OmHdf5Path & path, OmSegment** page, OmSegmentCache* cache ) 
{
	int size;
	char* p_data = (char*) OmProjectData::GetProjectDataReader()->dataset_raw_read(path, &size);
	
	QByteArray ba = QByteArray::fromRawData( p_data, size );
	QDataStream in(&ba, QIODevice::ReadOnly);
	in.setByteOrder( QDataStream::LittleEndian );
	in.setVersion(QDataStream::Qt_4_6);

	bool valid;
        for( quint32 i = 0; i < cache->getPageSize(); ++i ){
                in >> valid;
		if (!valid) {
                	page[ i ] = NULL;
			continue;
		}

                OmSegment * segment = new OmSegment(cache);

                in >> segment->mValue;
                in >> segment->mColorInt.red;
                in >> segment->mColorInt.green;
                in >> segment->mColorInt.blue;

                page[ i ] = segment;
        }

	delete p_data;
}

void OmDataArchiveSegment::ArchiveWrite( const OmHdf5Path & path, OmSegment** page, OmSegmentCache* cache) 
{
	QByteArray ba;
	QDataStream out(&ba, QIODevice::WriteOnly);
	out.setByteOrder( QDataStream::LittleEndian );
	out.setVersion(QDataStream::Qt_4_6);

        for( quint32 i = 0; i < cache->getPageSize(); ++i ){
        	OmSegment * segment = page[ i ];

		if (NULL == segment) {
			out << false;
			continue;
		} 

		out << true;

                out << segment->mValue;
                out << segment->mColorInt.red;
                out << segment->mColorInt.green;
                out << segment->mColorInt.blue;
        }
	
	OmProjectData::GetDataWriter()->dataset_raw_create_tree_overwrite( path, 
									   ba.size(), 
									   ba.data() );
}
