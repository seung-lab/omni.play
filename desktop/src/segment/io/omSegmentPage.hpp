#pragma once

#include "segment/io/omSegmentPageV1.hpp"
#include "segment/io/omSegmentPageV2.hpp"
#include "segment/io/omSegmentPageV3.hpp"
#include "segment/io/omSegmentPageV4.hpp"
#include "segment/io/omSegmentPageVersion.hpp"
#include "segment/io/omSegmentPageObjects.hpp"
#include "segment/io/omSegmentPageConverter.hpp"
#include "segment/io/omSegmentListTypePage.hpp"

class OmSegmentPage {
 private:
  OmSegmentation* segmentation_;
  OmSegments* segments_;
  om::common::PageNum pageNum_;
  uint32_t pageSize_;

  std::shared_ptr<OmSegmentPageVersion> versionInfo_;

  std::shared_ptr<OmSegmentPageObjects> objectPoolPtr_;
  OmSegment* objectPool_;

  std::shared_ptr<OmSegmentPageV4> segmentsDataPtr_;
  OmSegmentDataV4* segmentsData_;

  std::shared_ptr<OmSegmentListTypePage> listTypePagePtr_;
  uint8_t* listTypePage_;

 public:
  OmSegmentPage()
      : segmentation_(NULL),
        segments_(NULL),
        pageNum_(0),
        pageSize_(0),
        objectPool_(NULL),
        segmentsData_(NULL),
        listTypePage_(NULL) {}

  OmSegmentPage(OmSegmentation* segmentation, const om::common::PageNum pageNum,
                const uint32_t pageSize)
      : segmentation_(segmentation),
        segments_(segmentation_->Segments()),
        pageNum_(pageNum),
        pageSize_(pageSize),
        objectPool_(NULL),
        segmentsData_(NULL),
        listTypePage_(NULL) {
    versionInfo_ =
        std::make_shared<OmSegmentPageVersion>(segmentation_, pageNum_);

    segmentsDataPtr_ =
        std::make_shared<OmSegmentPageV4>(segmentation_, pageNum_, pageSize_);

    listTypePagePtr_ = std::make_shared<OmSegmentListTypePage>(
        segmentation_, pageNum_, pageSize_);

    makeSegmentObjectPool();
  }

  void Flush() {
    segmentsDataPtr_->Flush();
    listTypePagePtr_->Flush();
  }

  void Create() {
    versionInfo_->SetAsLatest();

    segmentsData_ = segmentsDataPtr_->Create();
    listTypePage_ = listTypePagePtr_->Create();

    for (uint32_t i = 0; i < pageSize_; ++i) {
      objectPool_[i].segments_ = segments_;
      objectPool_[i].data_ = &segmentsData_[i];
      objectPool_[i].listType_ = &listTypePage_[i];
      objectPool_[i].data_->bounds = om::dataBbox(segmentation_, 0);
    }
  }

  void Load() {
    versionInfo_->Load();

    loadSegmentsData();

    for (uint32_t i = 0; i < pageSize_; ++i) {
      objectPool_[i].segments_ = segments_;
      objectPool_[i].data_ = &segmentsData_[i];
      objectPool_[i].listType_ = &listTypePage_[i];
    }
  }

  inline OmSegment& operator[](const uint32_t index) {
    return objectPool_[index];
  }

  inline const OmSegment& operator[](const uint32_t index) const {
    return objectPool_[index];
  }

 private:
  void makeSegmentObjectPool() {
    objectPoolPtr_.reset(new OmSegmentPageObjects(pageSize_));

    // objectPool_ = objectPoolPtr_->MakeSegmentObjectPoolInMemory();
    objectPool_ = objectPoolPtr_->MakeSegmentObjectPoolOnDisk();

    for (uint32_t i = 0; i < pageSize_; ++i) {
      objectPool_[i].edgeNumber_ = -1;
    }
  }

  void loadSegmentsData() {
    switch (versionInfo_->Get()) {
      case 0:
        throw om::ArgException("bad segment page version number");
      case 1:
        convertFromHDF5();
        break;
      case 2:
        convertFromVersion2();
        break;
      case 3:
        convertFromVersion3();
        break;
      case 4:
        loadV4();
        break;
      default:
        throw om::ArgException("unknown segment page version number: " +
                               om::string::num(versionInfo_->Get()));
    }
  }

  template <class C> void convert(C& page) {
    std::shared_ptr<OmSegmentDataV3> dataV3 = page.Read();

    std::shared_ptr<OmSegmentDataV4> dataV4 =
        OmSegmentPageConverter::ConvertPageV3toV4(dataV3, pageSize_);
    segmentsData_ = segmentsDataPtr_->Import(dataV4);

    std::shared_ptr<uint8_t> listType =
        OmSegmentPageConverter::ConvertPageV3toV4ListType(dataV3, pageSize_);
    listTypePage_ = listTypePagePtr_->Import(listType);

    versionInfo_->SetAsLatest();
  }

  void convertFromHDF5() {
    OmSegmentPageV1 page(segmentation_, pageNum_, pageSize_);
    convert(page);
  }

  void convertFromVersion2() {
    OmSegmentPageV2 page(segmentation_, pageNum_, pageSize_);
    convert(page);
  }

  void convertFromVersion3() {
    OmSegmentPageV3 page(segmentation_, pageNum_, pageSize_);
    convert(page);
  }

  void loadV4() {
    segmentsData_ = segmentsDataPtr_->Load();
    listTypePage_ = listTypePagePtr_->Load();
  }
};
