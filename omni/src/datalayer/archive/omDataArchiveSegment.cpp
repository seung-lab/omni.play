#include "common/omException.h"
#include "datalayer/archive/omDataArchiveCoords.h"
#include "datalayer/archive/omDataArchiveSegment.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omIDataReader.h"
#include "datalayer/omIDataWriter.h"
#include "segment/omSegment.h"
#include "segment/omSegmentCache.h"
#include "system/omProjectData.h"
#include <QDataStream>

static const QString Omni_Postfix("OMNI");
const int Omni_Segment_Version = 2;
static int segmentFileVersion_;

void OmDataArchiveSegment::ArchiveRead(const OmDataPath & path,
									   std::vector<OmSegment> & page,
									   boost::shared_ptr<OmSegmentCache> cache,
									   const om::RewriteSegments rewriteSegments)
{
	OmDataArchiveSegment dsw(path, page, cache);
	dsw.archiveRead(rewriteSegments);
}

OmDataArchiveSegment::OmDataArchiveSegment(const OmDataPath & path,
										   std::vector<OmSegment> & page,
										   boost::shared_ptr<OmSegmentCache> cache)
	: path_(path)
	, page_(page)
	, cache_(cache)
	, omniFileVersion_(OmProjectData::getFileVersion())
	, size_(0)
{
}

void OmDataArchiveSegment::archiveRead(const om::RewriteSegments rewriteSegments)
{
	dw_ = OmProjectData::GetProjectIDataReader()->readDataset(path_, &size_);

	if(omniFileVersion_ >= 15){
		readSegmentsNew();
		return;
	}

	attemptOldSegmentRead();

	if(om::REWRITE_SEGMENTS == rewriteSegments){
		printf("forcing rewrite of segments...\n");
		archiveWrite();
	}
}

void OmDataArchiveSegment::attemptOldSegmentRead()
{
	// segments weren't versioned

	printf("segment load: file version is %d\n", omniFileVersion_);
	const bool dataReadCorrect = readSegmentsOld(false);

	if(!dataReadCorrect){
		// reread, don't read mBounds
		// warning: will leak corrupt OmSegments...
		printf("intial segment load failed; rereading\n");
		const bool dataReadCorrectOverride = readSegmentsOld(true);
		if(!dataReadCorrectOverride){
			throw OmIoException("corrupt segment list detected");
		}
	}
}

void OmDataArchiveSegment::archiveWrite()
{
	ArchiveWrite(path_, page_, cache_);
}

// don't modify this--to add extra member vars, please modify readSegmentsNew
bool OmDataArchiveSegment::readSegmentsOld(const bool overrideVersion)
{
	QByteArray ba = QByteArray::fromRawData( dw_->getPtr<char>(), size_ );
	QDataStream in(&ba, QIODevice::ReadOnly);
	in.setByteOrder( QDataStream::LittleEndian );
	in.setVersion(QDataStream::Qt_4_6);

	for(uint32_t i = 0; i < cache_->getPageSize(); ++i ){
		bool valid;
		in >> valid;
		if (!valid) {
			continue;
		}

		page_[i].cache_ = cache_;
		in >> page_[i].value_;
		in >> page_[i].colorInt_.red;
		in >> page_[i].colorInt_.green;
		in >> page_[i].colorInt_.blue;
		in >> page_[i].immutable_;
		in >> page_[i].size_;

		if(!overrideVersion && omniFileVersion_ >= 13){
			in >> page_[i].bounds_;
		}
	}

	if(in.atEnd()){
		return true; // segment version is good
	}
	return false; // assume we're in an inconsistent version state...
}

void OmDataArchiveSegment::readSegmentsNew()
{
	QByteArray ba = QByteArray::fromRawData( dw_->getPtr<char>(), size_ );
	QDataStream in(&ba, QIODevice::ReadOnly);
	in.setByteOrder( QDataStream::LittleEndian );
	in.setVersion(QDataStream::Qt_4_6);

	in >> segmentFileVersion_;
	printf("segment load: segment version is %d\n", segmentFileVersion_);

	for(uint32_t i = 0; i < cache_->getPageSize(); ++i ){
		bool valid;
		in >> valid;
		if (!valid) {
			continue;
		}

		page_[i].cache_ = cache_;
		in >> page_[i].value_;
		in >> page_[i].colorInt_.red;
		in >> page_[i].colorInt_.green;
		in >> page_[i].colorInt_.blue;
		in >> page_[i].immutable_;
		in >> page_[i].size_;
		in >> page_[i].bounds_;
	}

	QString omniPostfix;
	in >> omniPostfix;
	if(Omni_Postfix != omniPostfix || !in.atEnd()){
		throw OmIoException("corrupt segment list detected");
	}
}

void OmDataArchiveSegment::ArchiveWrite(const OmDataPath & path,
										const std::vector<OmSegment> & page,
										boost::shared_ptr<OmSegmentCache> cache)
{
	const int omniFileVersion = OmProjectData::getFileVersion();

	QByteArray ba;
	QDataStream out(&ba, QIODevice::WriteOnly);
	out.setByteOrder( QDataStream::LittleEndian );
	out.setVersion(QDataStream::Qt_4_6);

	if(0 == omniFileVersion){
		assert(0 && "file version not set");
	}

	out << Omni_Segment_Version;

	for(uint32_t i = 0; i < cache->getPageSize(); ++i ){
		if(0 == page[i].value_){
			out << false;
			continue;
		}

		out << true;

		out << page[i].value_;
		out << page[i].colorInt_.red;
		out << page[i].colorInt_.green;
		out << page[i].colorInt_.blue;
		out << page[i].immutable_;
		out << page[i].size_;
		out << page[i].bounds_;
	}

	out << Omni_Postfix;

	OmProjectData::GetIDataWriter()->
		writeDataset( path,
					  ba.size(),
					  OmDataWrapperRaw(ba.constData()));

	OmProjectData::GetIDataWriter()->flush();
}
