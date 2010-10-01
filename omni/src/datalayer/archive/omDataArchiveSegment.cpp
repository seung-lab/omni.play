#include "common/omException.h"
#include "datalayer/archive/omDataArchiveCoords.h"
#include "datalayer/archive/omDataArchiveSegment.h"
#include "datalayer/archive/omDataArchiveVmml.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omIDataReader.h"
#include "datalayer/omIDataWriter.h"
#include "segment/omSegment.h"
#include "segment/omSegmentCache.h"
#include "system/omProjectData.h"

#include <QDataStream>

static const QString Omni_Postfix("OMNI");
const int Omni_Segment_Version = 2;

void OmDataArchiveSegment::ArchiveRead( const OmDataPath & path,
										std::vector<OmSegment*> & page,
										boost::shared_ptr<OmSegmentCache> cache)
{
	int size;
	OmDataWrapperPtr dw = OmProjectData::GetProjectIDataReader()->readDataset(path, &size);

	printf("segment load: file version is %d\n",
		   OmProjectData::getFileVersion());

	if(OmProjectData::getFileVersion() < 15){ // segments weren't versioned
		const bool dataReadCorrect =
			readSegmentsOld(page, cache, dw, size, false);

		if(!dataReadCorrect){
			// reread, don't read mBounds
			// warning: will leak corrupt OmSegments...
			const bool dataReadCorrectOverride =
				readSegmentsOld(page, cache, dw, size, true);
			if(!dataReadCorrectOverride){
				throw OmIoException("corrupt segment list detected");
			}
			printf("forcing rewrite of segments...\n");
			ArchiveWrite(path, page, cache);
		}
	}else{
		readSegmentsNew(page, cache, dw, size);
	}
}

// don't modify this--to add extra member vars, please modify readSegmentsNew
bool OmDataArchiveSegment::readSegmentsOld(std::vector<OmSegment*> & page,
										   boost::shared_ptr<OmSegmentCache> cache,
										   OmDataWrapperPtr dw,
										   const int size,
										   const bool overrideVersion)
{
	QByteArray ba = QByteArray::fromRawData( dw->getPtr<char>(), size );
	QDataStream in(&ba, QIODevice::ReadOnly);
	in.setByteOrder( QDataStream::LittleEndian );
	in.setVersion(QDataStream::Qt_4_6);

	for(uint32_t i = 0; i < cache->getPageSize(); ++i ){
		bool valid;
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
										   boost::shared_ptr<OmSegmentCache> cache,
										   OmDataWrapperPtr dw,
										   const int size)
{
	QByteArray ba = QByteArray::fromRawData( dw->getPtr<char>(), size );
	QDataStream in(&ba, QIODevice::ReadOnly);
	in.setByteOrder( QDataStream::LittleEndian );
	in.setVersion(QDataStream::Qt_4_6);

	int segmentFileVersion_;
	in >> segmentFileVersion_;

	for(uint32_t i = 0; i < cache->getPageSize(); ++i ){
		bool valid;
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
		in >> segment->mBounds;

		page[ i ] = segment;
	}

	QString omniPostfix;
	in >> omniPostfix;
	if( Omni_Postfix != omniPostfix || !in.atEnd() ){
		throw OmIoException("corrupt segment list detected");
	}
}

void OmDataArchiveSegment::ArchiveWrite(const OmDataPath & path,
										const std::vector<OmSegment*> & page,
										boost::shared_ptr<OmSegmentCache> cache)
{
	QByteArray ba;
	QDataStream out(&ba, QIODevice::WriteOnly);
	out.setByteOrder( QDataStream::LittleEndian );
	out.setVersion(QDataStream::Qt_4_6);

	if(0 == OmProjectData::getFileVersion()){
		assert(0 && "file version not set");
	}

	if(OmProjectData::getFileVersion() >= 15){
		out << Omni_Segment_Version;
	}

	for(uint32_t i = 0; i < cache->getPageSize(); ++i ){
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

	OmProjectData::GetIDataWriter()->
		writeDataset( path,
			      ba.size(),
			      OmDataWrapperRaw(ba.constData()));
}
