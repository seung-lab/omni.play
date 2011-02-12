#ifndef OM_SEGMENT_PAGE_HPP
#define OM_SEGMENT_PAGE_HPP

#include "common/omCommon.h"
#include "datalayer/archive/omDataArchiveSegment.h"
#include "datalayer/fs/omFileNames.hpp"
#include "datalayer/fs/omIOnDiskFile.h"
#include "datalayer/omDataPath.h"
#include "datalayer/omDataPaths.h"
#include "segment/omSegment.h"
#include "utility/dataWrappers.h"
#include "utility/omSmartPtr.hpp"
#include "datalayer/fs/omFileQT.hpp"

#include <QFile>
#include <QDir>

class OmSegmentPage {
private:
	// version 1: pages in hdf5
	// version 2: first move to mem-mapped pages
	// version 3: replace 'bool immutable' w/ 'enum OmSegListType'
	static const int CurrentFileVersion = 3;

	OmSegmentation* segmentation_;
	PageNum pageNum_;
	uint32_t pageSize_;
	uint32_t version_;

	OmSegmentCache* cache_;

	boost::shared_ptr<OmSegment> segmentsPtr_;
	OmSegment* segments_;

	boost::shared_ptr<OmIOnDiskFile<OmSegmentDataV3> > segmentsDataPtr_;
	OmSegmentDataV3* segmentsData_;

	typedef OmFileReadQT<OmSegmentDataV2> reader_t_v2;
	typedef OmFileReadQT<OmSegmentDataV3> reader_t_v3;
	typedef OmFileWriteQT<OmSegmentDataV2> writer_t_v2;
	typedef OmFileWriteQT<OmSegmentDataV3> writer_t_v3;

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
		, cache_(segmentation_->SegmentCache())
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

		segmentsDataPtr_ = writer_t_v3::WriterNumElements(memMapPathV3(),
														  pageSize_,
														  om::ZERO_FILL);
		segmentsData_ = segmentsDataPtr_->GetPtr();

		for(uint32_t i = 0; i < pageSize_; ++i){
			segments_[i].cache_ = cache_;
			segments_[i].data_ = &segmentsData_[i];
			segments_[i].data_->bounds = DataBbox();
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
			convertFromVersion2();
			return;
		case 3:
			openMemMapFile();
			return;
		default:
			throw OmArgException("unknown segment page version number");
		}
	}

	inline OmSegment& operator[](const uint32_t index)
	{
		assert(pageSize_ && index < pageSize_);
		return segments_[index];
	}

	inline const OmSegment& operator[](const uint32_t index) const
	{
		assert(pageSize_ && index < pageSize_);
		return segments_[index];
	}

private:
	void makeSegmentObjectPool()
	{
		segmentsPtr_ = OmSmartPtr<OmSegment>::NewNumElements(pageSize_);
		segments_ = segmentsPtr_.get();
	}

	void convertFromVersion2()
	{
		printf("rewriting segment page %d from ver2 to ver3\n", pageNum_);

		boost::shared_ptr<OmIOnDiskFile<OmSegmentDataV2> > oldSegmentDataPtr =
			reader_t_v2::Reader(memMapPathV2());
		OmSegmentDataV2* oldSegmentData = oldSegmentDataPtr->GetPtr();

		Create();

		for(uint32_t i = 0; i < pageSize_; ++i){
			segmentsData_[i].value = oldSegmentData[i].value;
			segmentsData_[i].color = oldSegmentData[i].color;
			segmentsData_[i].size = oldSegmentData[i].size;
			segmentsData_[i].bounds = oldSegmentData[i].bounds;

			if(oldSegmentData[i].immutable){
				segmentsData_[i].listType = om::VALID;
			} else {
				segmentsData_[i].listType = om::WORKING;
			}
		}

		Flush();
	}

	void convertFromHDF5()
	{
		printf("rewriting segment page %d from HDF5\n", pageNum_);

		Create();

		boost::shared_ptr<OmSegmentDataV2> oldSegmentDataPtr =
			OmSmartPtr<OmSegmentDataV2>::MallocNumElements(pageSize_,
														  om::ZERO_FILL);
		OmSegmentDataV2* oldSegmentData = oldSegmentDataPtr.get();

		OmDataArchiveSegment::ArchiveRead(getOldHDF5path(),
										  oldSegmentData,
										  pageSize_);

		for(uint32_t i = 0; i < pageSize_; ++i){
			segmentsData_[i].value = oldSegmentData[i].value;
			segmentsData_[i].color = oldSegmentData[i].color;
			segmentsData_[i].size = oldSegmentData[i].size;
			segmentsData_[i].bounds = oldSegmentData[i].bounds;

			if(oldSegmentData[i].immutable){
				segmentsData_[i].listType = om::VALID;
			} else {
				segmentsData_[i].listType = om::WORKING;
			}
		}

		Flush();
	}

	void openMemMapFile()
	{
		segmentsDataPtr_ = reader_t_v3::Reader(memMapPathV3());
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

	std::string memMapPathV2(){
		return memMapPathQStrV2().toStdString();
	}

	QString memMapPathQStrV2(){
		const QString volPath = OmFileNames::MakeVolSegmentsPath(segmentation_);
		const QString fullPath = QString("%1segment_page%2.%3")
			.arg(volPath)
			.arg(pageNum_)
			.arg("data");

		return fullPath;
	}

	std::string memMapPathV3(){
		return memMapPathQStrV3().toStdString();
	}

	QString memMapPathQStrV3(){
		const QString volPath = OmFileNames::MakeVolSegmentsPath(segmentation_);
		const QString fullPath = QString("%1segment_page%2.%3.ver3")
			.arg(volPath)
			.arg(pageNum_)
			.arg("data");

		return fullPath;
	}

	QString versionFilePath(){
		return memMapPathQStrV2() + ".ver";
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
