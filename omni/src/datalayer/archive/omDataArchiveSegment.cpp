#include "datalayer/archive/omDataArchiveSegment.h"
#include "datalayer/archive/omDataArchiveCoords.h"
#include "datalayer/omDataReader.h"
#include "datalayer/omDataWriter.h"
#include "datalayer/omDataPath.h"
#include "segment/omSegment.h"
#include "system/omProjectData.h"
#include "segment/omSegmentCache.h"

#include <QDataStream>

void OmDataArchiveSegment::ArchiveRead( const OmDataPath & path, std::vector<OmSegment*> & page, OmSegmentCache* cache ) 
{
	int size;
	OmDataWrapperPtr dw = OmProjectData::GetProjectDataReader()->dataset_raw_read(path, &size);
	
	QByteArray ba = QByteArray::fromRawData( dw->getPtr<char>(), size );
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
                in >> segment->mImmutable;
                in >> segment->mSize;

                page[ i ] = segment;
        }
}

void OmDataArchiveSegment::ArchiveWrite( const OmDataPath & path, const std::vector<OmSegment*> & page, OmSegmentCache* cache) 
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
                out << segment->mImmutable;
                out << segment->mSize;
        }
	
	OmProjectData::GetDataWriter()->dataset_raw_create_tree_overwrite( path, 
									   ba.size(), 
									   OmDataWrapperRaw(ba.data()));
}
