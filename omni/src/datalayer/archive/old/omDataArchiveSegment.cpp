#include "common/omException.h"
#include "datalayer/archive/old/omDataArchiveSegment.h"
#include "segment/omSegment.h"
#include "segment/omSegments.h"
#include "segment/io/omSegmentPage.hpp"
#include "project/omProject.h"
#include "datalayer/hdf5/omHdf5.h"

#include <QDataStream>

static const QString Omni_Postfix("OMNI");
const int Omni_Segment_Version = 2;
static int segmentFileVersion_;

void OmDataArchiveSegment::ArchiveRead(const OmDataPath& path,
									   OmSegmentDataV2* page,
									   const uint32_t pageSize)
{
	OmDataArchiveSegment dsw(path, page, pageSize);
	dsw.archiveRead();
}

OmDataArchiveSegment::OmDataArchiveSegment(const OmDataPath & path,
										   OmSegmentDataV2* page,
										   const uint32_t pageSize)
	: path_(path)
	, page_(page)
	, pageSize_(pageSize)
	, omniFileVersion_(OmProject::GetFileVersion())
{
}

void OmDataArchiveSegment::archiveRead()
{
	OmHdf5* reader = OmProject::OldHDF5();
	dw_ = reader->readDataset(path_, &size_);

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
			page_[i].value = 0;
			continue;
		}

		in >> page_[i].value;
		in >> page_[i].color.red;
		in >> page_[i].color.green;
		in >> page_[i].color.blue;
		in >> page_[i].immutable;
		quint64 size;
		in >> size;
		page_[i].size = size;

		if(!overrideVersion && omniFileVersion_ >= 13){
			in >> page_[i].bounds;
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
			page_[i].value = 0;
			continue;
		}

		in >> page_[i].value;
		in >> page_[i].color.red;
		in >> page_[i].color.green;
		in >> page_[i].color.blue;
		in >> page_[i].immutable;
		quint64 size;
		in >> size;
		page_[i].size = size;
		in >> page_[i].bounds;
	}

	QString omniPostfix;
	in >> omniPostfix;
	if(Omni_Postfix != omniPostfix || !in.atEnd()){
		throw OmIoException("corrupt segment list detected");
	}
}
