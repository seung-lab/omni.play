#ifndef OM_SEGMENT_PAGE_HPP
#define OM_SEGMENT_PAGE_HPP

#include "common/omCommon.h"
#include "datalayer/archive/omDataArchiveSegment.h"
#include "datalayer/fs/omFileNames.hpp"
#include "datalayer/fs/omIOnDiskFile.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataPaths.h"
#include "segment/omSegment.h"
#include "system/omProjectData.h"
#include "utility/dataWrappers.h"
#include "utility/omSmartPtr.hpp"
#include "datalayer/fs/omFileQT.hpp"

#include <QFile>
#include <QDir>

class OmSegmentPage {
private:
	// version 1: pages in hdf5
	// version 2: first move to mem-mapped pages
	static const int CurrentFileVersion = 2;

	OmSegmentation* segmentation_;
	PageNum pageNum_;
	uint32_t pageSize_;
	uint32_t version_;

	OmSegmentCache* cache_;

	boost::shared_ptr<OmSegment> segmentsPtr_;
	OmSegment* segments_;

	boost::shared_ptr<OmIOnDiskFile<OmSegmentData> > segmentsDataPtr_;
	OmSegmentData* segmentsData_;

	typedef OmFileReadQT<OmSegmentData> reader_t;
	typedef OmFileWriteQT<OmSegmentData> writer_t;

public:
	OmSegmentPage()
		: segmentation_(NULL)
		, pageNum_(0)
		, pageSize_(0)
		, version_(0)
		, cache_(NULL)
		, segments_(NULL)
		, segmentsData_(NULL)
	{}

	OmSegmentPage(OmSegmentation* segmentation, const PageNum pageNum,
				  const uint32_t pageSize)
		: segmentation_(segmentation)
		, pageNum_(pageNum)
		, pageSize_(pageSize)
		, version_(0)
		, cache_(segmentation_->GetSegmentCache())
		, segments_(NULL)
		, segmentsData_(NULL)
	{
		makeSegmentObjectPool();
	}

	void Flush()
	{
		segmentsDataPtr_->Flush();
	}

	void Create()
	{
		version_ = CurrentFileVersion;
		storeVersion();

		const int segmentDataFileSize = pageSize_*sizeof(OmSegmentData);
		segmentsDataPtr_ = boost::make_shared<writer_t>(memMapPath(),
													 segmentDataFileSize,
													 om::ZERO_FILL);
		segmentsData_ = segmentsDataPtr_->GetPtr();

		for(uint32_t i = 0; i < pageSize_; ++i){
			segments_[i].cache_ = cache_;
			segments_[i].data_ = &segmentsData_[i];
			segments_[i].data_->bounds_ = DataBbox();
		}
	}

	void Load()
	{
		loadVersion();

		switch(version_){
		case 0:
			throw OmArgException("bad segment page version number");
		case 1:
			convertFromHDF5();
			return;
		case 2:
			openMemMapFile();
			return;
		default:
			throw OmArgException("unknown segment page version number");
		}
	}

	inline OmSegment& operator[](const uint32_t index){
		assert(pageSize_ && index < pageSize_);
		return segments_[index];
	}

	inline const OmSegment& operator[](const uint32_t index) const {
		assert(pageSize_ && index < pageSize_);
		return segments_[index];
	}

private:
	void makeSegmentObjectPool()
	{
		segmentsPtr_ = OmSmartPtr<OmSegment>::NewNumElements(pageSize_);
		segments_ = segmentsPtr_.get();
	}

	void convertFromHDF5()
	{
		printf("rewriting segment page %d\n", pageNum_);
		Create();
		OmDataArchiveSegment::ArchiveRead(getOldHDF5path(),
										  segmentsData_,
										  pageSize_);
	}

	void openMemMapFile()
	{
		segmentsDataPtr_ = boost::make_shared<reader_t>(memMapPath(), 0);
		segmentsData_ = segmentsDataPtr_->GetPtr();

		for(uint32_t i = 0; i < pageSize_; ++i){
			segments_[i].cache_ = cache_;
			segments_[i].data_ = &segmentsData_[i];
		}
	}

	OmDataPath getOldHDF5path(){
		return OmDataPaths::getSegmentPagePath(segmentation_->GetID(),
											   pageNum_);
	}

	std::string memMapPath(){
		return memMapPathQStr().toStdString();
	}

	QString memMapPathQStr(){
		const QString volPath = OmFileNames::MakeVolSegmentsPath(segmentation_);
		const QString fullPath = QString("%1/segment_page%2.%3")
			.arg(volPath)
			.arg(pageNum_)
			.arg("data");

		return fullPath;
	}

	QString versionFilePath(){
		return memMapPathQStr() + ".ver";
	}

	void loadVersion()
	{
		QFile file(versionFilePath());

		if(!file.exists()){
			version_ = 1;
			return;
		}

		if(!file.open(QIODevice::ReadOnly)){
			throw OmIoException("error reading file", versionFilePath());
		}

		QDataStream in(&file);
		in.setByteOrder( QDataStream::LittleEndian );
		in.setVersion(QDataStream::Qt_4_6);

		in >> version_;

		if(!in.atEnd()){
			throw OmIoException("corrupt file?", versionFilePath());
		}
	}

	void storeVersion()
	{
		QFile file(versionFilePath());

		if (!file.open(QIODevice::WriteOnly)) {
			throw OmIoException("could not write file", versionFilePath());
		}

		QDataStream out(&file);
		out.setByteOrder( QDataStream::LittleEndian );
		out.setVersion(QDataStream::Qt_4_6);

		out << version_;
	}
};

#endif
