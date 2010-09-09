#include "common/omException.h"
#include "datalayer/archive/omDataArchiveCoords.h"
#include "datalayer/archive/omDataArchiveSegment.h"
#include "datalayer/archive/omDataArchiveVmml.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataReader.h"
#include "datalayer/omDataWriter.h"
#include "segment/omSegment.h"
#include "segment/omSegmentCache.h"
#include "system/omProjectData.h"

#include <QDataStream>

const int Omni_Segment_Version = 2;

void OmDataArchiveSegment::ArchiveRead(const OmDataPath & path,
				       std::vector<OmSegment*> & page,
				       OmSegmentCache* cache)
{
	int size;
	OmDataWrapperPtr dw = OmProjectData::GetProjectDataReader()->dataset_raw_read(path, &size);

	if(OmProjectData::getFileVersion() < 15){ // segments weren't versioned
		const bool dataReadCorrect = readSegmentsOld(page, cache, dw, size, false);
		if(!dataReadCorrect){
			readSegmentsOld(page, cache, dw, size, true);
			printf("forcing rewrite of segments...\n");
			ArchiveWrite(path, page, cache);
		}
	}else{
		readSegmentsNew(page, cache, dw, size);
	}
}

bool OmDataArchiveSegment::readSegmentsOld(std::vector<OmSegment*> & page,
					   OmSegmentCache* cache,
					   OmDataWrapperPtr dw,
					   const int size,
					   const bool overrideVersion)
{
	QByteArray ba = QByteArray::fromRawData( dw->getCharPtr(), size );
	QDataStream in(&ba, QIODevice::ReadOnly);
	in.setByteOrder( QDataStream::LittleEndian );
	in.setVersion(QDataStream::Qt_4_6);

        for( quint32 i = 0; i < cache->getPageSize(); ++i ){
		bool valid;
                in >> valid;
		if (!valid) {
                	page[ i ] = NULL;
			continue;
		}

                OmSegment* segment = new OmSegment(cache);

                in >> segment->mValue;
                in >> segment->mColorInt.red;
                in >> segment->mColorInt.green;
                in >> segment->mColorInt.blue;
                in >> segment->mImmutable;
                in >> segment->mSize;

		if(!overrideVersion && OmProjectData::getFileVersion() >= 13) {
			in >> segment->mBounds;
		}

                page[ i ] = segment;
        }

	if(in.atEnd()){
		return true; // segment version is good
	}
	return false; // assume we're in an inconsistent version state...
}

void OmDataArchiveSegment::readSegmentsNew(std::vector<OmSegment*> & page,
					   OmSegmentCache* cache,
					   OmDataWrapperPtr dw,
					   const int size)
{
	QByteArray ba = QByteArray::fromRawData( dw->getCharPtr(), size );
	QDataStream in(&ba, QIODevice::ReadOnly);
	in.setByteOrder( QDataStream::LittleEndian );
	in.setVersion(QDataStream::Qt_4_6);

	int segmentFileVersion_;
	in >> segmentFileVersion_;

        for( quint32 i = 0; i < cache->getPageSize(); ++i ){
		bool valid;
                in >> valid;
		if (!valid) {
                	page[ i ] = NULL;
			continue;
		}

                OmSegment* segment = new OmSegment(cache);

                in >> segment->mValue;
                in >> segment->mColorInt.red;
                in >> segment->mColorInt.green;
                in >> segment->mColorInt.blue;
                in >> segment->mImmutable;
                in >> segment->mSize;
		in >> segment->mBounds;

                page[ i ] = segment;
        }

	if(!in.atEnd()){
		throw OmIoException("corrupt segment list detected");
	}
}

void OmDataArchiveSegment::ArchiveWrite(const OmDataPath & path,
					const std::vector<OmSegment*> & page,
					OmSegmentCache* cache)
{
	QByteArray ba;
	QDataStream out(&ba, QIODevice::WriteOnly);
	out.setByteOrder( QDataStream::LittleEndian );
	out.setVersion(QDataStream::Qt_4_6);

	if(OmProjectData::getFileVersion() >= 15){
		out << Omni_Segment_Version;
	}

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
		out << segment->mBounds;
        }

	OmProjectData::GetDataWriter()->dataset_raw_create_tree_overwrite( path,
									   ba.size(),
									   ba.data() );
}
