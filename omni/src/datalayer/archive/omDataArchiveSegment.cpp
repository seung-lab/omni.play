#include "common/omException.h"
#include "datalayer/archive/omDataArchiveCoords.h"
#include "datalayer/archive/omDataArchiveSegment.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omIDataReader.h"
#include "datalayer/omIDataWriter.h"
#include "segment/omSegment.h"
#include "segment/omSegmentCache.h"
#include "segment/lowLevel/omSegmentPage.hpp"
#include "system/omProjectData.h"
#include <QDataStream>

static const QString Omni_Postfix("OMNI");
const int Omni_Segment_Version = 2;
static int segmentFileVersion_;

void OmDataArchiveSegment::ArchiveRead(const OmDataPath& path,
									   OmSegmentData* page,
									   const uint32_t pageSize)
{
	OmDataArchiveSegment dsw(path, page, pageSize);
	dsw.archiveRead();
}

OmDataArchiveSegment::OmDataArchiveSegment(const OmDataPath & path,
										   OmSegmentData* page,
										   const uint32_t pageSize)
	: path_(path)
	, page_(page)
	, pageSize_(pageSize)
	, omniFileVersion_(OmProjectData::getFileVersion())
{
}

void OmDataArchiveSegment::archiveRead()
{
	dw_ = OmProjectData::GetProjectIDataReader()->readDataset(path_, &size_);

	if(omniFileVersion_ >= 15){
		readSegmentsNew();
		return;
	}

	attemptOldSegmentRead();
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

// don't modify this--to add extra member vars, please modify readSegmentsNew
bool OmDataArchiveSegment::readSegmentsOld(const bool overrideVersion)
{
	QByteArray ba = QByteArray::fromRawData( dw_->getPtr<char>(), size_ );
	QDataStream in(&ba, QIODevice::ReadOnly);
	in.setByteOrder( QDataStream::LittleEndian );
	in.setVersion(QDataStream::Qt_4_6);

	for(uint32_t i = 0; i < pageSize_; ++i ){
		bool valid;
		in >> valid;
		if (!valid) {
			page_[i].value_ = 0;
			continue;
		}

		in >> page_[i].value_;
		in >> page_[i].color_.red;
		in >> page_[i].color_.green;
		in >> page_[i].color_.blue;
		in >> page_[i].immutable_;
		quint64 size;
		in >> size;
		page_[i].size_ = size;

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

	for(uint32_t i = 0; i < pageSize_; ++i ){
		bool valid;
		in >> valid;
		if (!valid) {
			page_[i].value_ = 0;
			continue;
		}

		in >> page_[i].value_;
		in >> page_[i].color_.red;
		in >> page_[i].color_.green;
		in >> page_[i].color_.blue;
		in >> page_[i].immutable_;
		quint64 size;
		in >> size;
		page_[i].size_ = size;
		in >> page_[i].bounds_;
	}

	QString omniPostfix;
	in >> omniPostfix;
	if(Omni_Postfix != omniPostfix || !in.atEnd()){
		throw OmIoException("corrupt segment list detected");
	}
}
