#include "datalayer/archive/omDataArchiveSegment.h"
#include "datalayer/archive/omDataArchiveVmml.h"
#include "datalayer/archive/omDataArchiveCoords.h"
#include "datalayer/omDataReader.h"
#include "datalayer/omDataWriter.h"
#include "datalayer/omDataPath.h"
#include "segment/omSegment.h"
#include "system/omProjectData.h"
#include "segment/omSegmentCache.h"

#include <QDataStream>

extern int Omni_File_Version;



void OmDataArchiveSegment::ArchiveRead( const OmDataPath & path, std::vector<OmSegment*> & page, OmSegmentCache* cache )
{
	int size;
	OmDataWrapperPtr dw = OmProjectData::GetProjectDataReader()->readDataset(path, &size);

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

		OmSegID val;
                in >> val;
                OmSegment * segment = new OmSegment(val, cache);
                in >> segment->mColorInt.red;
                in >> segment->mColorInt.green;
                in >> segment->mColorInt.blue;
                in >> segment->mImmutable;
                in >> segment->mSize;

		if(Omni_File_Version >= 13) {
			in >> segment->mBounds;
		}

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

                out << segment->value;
                out << segment->mColorInt.red;
                out << segment->mColorInt.green;
                out << segment->mColorInt.blue;
                out << segment->mImmutable;
                out << segment->mSize;
		out << segment->mBounds;
        }

	OmProjectData::GetDataWriter()->writeDataset( path,
									   ba.size(),
									   OmDataWrapperRaw(ba.data()));
}
