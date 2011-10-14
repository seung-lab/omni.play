#pragma once

#include "segment/io/segmentPageObjects.hpp"
#include "segment/io/segmentPageConverter.hpp"
#include "segment/io/segmentListTypePage.hpp"

class segmentPage {
private:
    segmentation* segmentation_;
    segments* segments_;
    PageNum pageNum_;
    uint32_t pageSize_;

    boost::shared_ptr<segmentPageObjects> objectPoolPtr_;
    segment* objectPool_;

    boost::shared_ptr<segmentPageV4> segmentsDataPtr_;
    segmentDataV4* segmentsData_;

    boost::shared_ptr<segmentListTypePage> listTypePagePtr_;
    uint8_t* listTypePage_;

public:
    segmentPage()
        : segmentation_(NULL)
        , segments_(NULL)
        , pageNum_(0)
        , pageSize_(0)
        , objectPool_(NULL)
        , segmentsData_(NULL)
        , listTypePage_(NULL)
    {}

    segmentPage(segmentation* segmentation, const PageNum pageNum,
                  const uint32_t pageSize)
        : segmentation_(segmentation)
        , segments_(segmentation_->Segments())
        , pageNum_(pageNum)
        , pageSize_(pageSize)
        , objectPool_(NULL)
        , segmentsData_(NULL)
        , listTypePage_(NULL)
    {
        segmentsDataPtr_ = boost::make_shared<segmentPageV4>(segmentation_,
                                                               pageNum_,
                                                               pageSize_);

        listTypePagePtr_ = boost::make_shared<segmentListTypePage>(segmentation_,
                                                                     pageNum_,
                                                                     pageSize_);

        makeSegmentObjectPool();
    }

    void Flush()
    {
        segmentsDataPtr_->Flush();
        listTypePagePtr_->Flush();
    }

    void Create()
    {
        versionInfo_->SetAsLatest();

        segmentsData_ = segmentsDataPtr_->Create();
        listTypePage_ = listTypePagePtr_->Create();

        for(uint32_t i = 0; i < pageSize_; ++i)
        {
            objectPool_[i].segments_ = segments_;
            objectPool_[i].data_ = &segmentsData_[i];
            objectPool_[i].listType_ = &listTypePage_[i];
            objectPool_[i].data_->bounds = coords::dataBbox(segmentation_, 0);
        }
    }

    void Load()
    {
        versionInfo_->Load();

        loadSegmentsData();

        for(uint32_t i = 0; i < pageSize_; ++i)
        {
            objectPool_[i].segments_ = segments_;
            objectPool_[i].data_ = &segmentsData_[i];
            objectPool_[i].listType_ = &listTypePage_[i];
        }
    }

    inline segment& operator[](const uint32_t index) {
        return objectPool_[index];
    }

    inline const segment& operator[](const uint32_t index) const {
        return objectPool_[index];
    }

private:
    void makeSegmentObjectPool()
    {
        objectPoolPtr_.reset(new segmentPageObjects(pageSize_));

        // objectPool_ = objectPoolPtr_->MakeSegmentObjectPoolInMemory();
        objectPool_ = objectPoolPtr_->MakeSegmentObjectPoolOnDisk();

        for(uint32_t i = 0; i < pageSize_; ++i){
            objectPool_[i].edgeNumber_ = -1;
        }
    }

    void loadSegmentsData()
    {
       switch(versionInfo_->Get()){
        case 0:
            throw common::argException("bad segment page version number");
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
            throw common::argException("unknown segment page version number: " +
                                 om::string::num(versionInfo_->Get()));
        }
    }

    template <class C>
    void convert(C& page)
    {
        boost::shared_ptr<segmentDataV3> dataV3 = page.Read();

        boost::shared_ptr<segmentDataV4> dataV4 =
            segmentPageConverter::ConvertPageV3toV4(dataV3, pageSize_);
        segmentsData_ = segmentsDataPtr_->Import(dataV4);

        boost::shared_ptr<uint8_t> listType =
            segmentPageConverter::ConvertPageV3toV4ListType(dataV3, pageSize_);
        listTypePage_ = listTypePagePtr_->Import(listType);

        versionInfo_->SetAsLatest();
    }

    void convertFromHDF5()
    {
        segmentPageV1 page(segmentation_, pageNum_, pageSize_);
        convert(page);
    }

    void convertFromVersion2()
    {
        segmentPageV2 page(segmentation_, pageNum_, pageSize_);
        convert(page);
    }

    void convertFromVersion3()
    {
        segmentPageV3 page(segmentation_, pageNum_, pageSize_);
        convert(page);
    }

    void loadV4()
    {
        segmentsData_ = segmentsDataPtr_->Load();
        listTypePage_ = listTypePagePtr_->Load();
    }
};

