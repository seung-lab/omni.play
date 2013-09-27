#pragma once

#include "utility/sortHelpers.h"
#include "volume/build/omVolumeBuilderHdf5.hpp"
#include "volume/build/omVolumeBuilderImages.hpp"
#include "volume/build/omVolumeBuilderWatershed.hpp"
#include "volume/build/omVolumeBuilderEmpty.hpp"

#include <QFileInfo>

template <typename VOL>
class OmVolumeBuilder {
private:
    VOL *const vol_;
    std::vector<QFileInfo> files_;
    const QString hdf5path_;

    enum ImportType {
        HDF5,
        IMAGES,
        WATERSHED,
        UNKNOWN
    };
    ImportType importType_;

public:
    OmVolumeBuilder(VOL* vol,
                    const std::vector<QFileInfo>& files,
                    const QString& hdf5path)
        : vol_(vol)
        , files_(files)
        , hdf5path_(hdf5path)
        , importType_(UNKNOWN)
    {
        setup();
    }

    OmVolumeBuilder(VOL* vol,
                    const std::vector<QFileInfo>& files)
        : vol_(vol)
        , files_(files)
        , hdf5path_("")
        , importType_(UNKNOWN)
    {
        setup();
    }

    OmVolumeBuilder(VOL* vol)
        : vol_(vol)
        , files_()
        , hdf5path_("")
        , importType_(UNKNOWN)
    {}

    void Build()
    {
        std::unique_ptr<OmVolumeBuilderBase<VOL> > builder(produceBuilder());
        builder->Build();
    }

    void Build(const om::common::AffinityGraph aff)
    {
        if(HDF5 != importType_){
            throw om::ArgException("first file to import is not HDF5");
        }

        OmVolumeBuilderHdf5<VOL> builder(vol_, files_[0], hdf5path_, aff);
        builder.Build();
    }

    void BuildWatershed()
    {
        if(WATERSHED != importType_){
            throw om::ArgException("first file to import is not Watershed");
        }

        OmVolumeBuilderWatershed<VOL> builder(vol_, files_[0]);
        builder.Build();
    }

    void BuildEmptyChannel()
    {
        OmVolumeBuilderEmpty<VOL> builder(vol_);
        builder.BuildEmpty();
    }

private:
    OmVolumeBuilderBase<VOL>* produceBuilder()
    {
        switch(importType_){
        case HDF5:
            return new OmVolumeBuilderHdf5<VOL>(vol_, files_[0], hdf5path_);
        case IMAGES:
            return new OmVolumeBuilderImages<VOL>(vol_, files_);
        case WATERSHED:
            return new OmVolumeBuilderWatershed<VOL>(vol_, files_[0]);
        default:
            throw om::ArgException("unknown type");
        };
    }

    void setup()
    {
        sortNaturally();
        setImportType();
        isSourceValid();
    }

    void sortNaturally(){
        SortHelpers::SortNaturally(files_);
    }

    void setImportType()
    {
        if(files_.empty()){
            throw om::IoException("no source files");
        }

        const QString fnp = files_[0].fileName();

        if(fnp.endsWith(".h5", Qt::CaseInsensitive) ||
           fnp.endsWith(".hdf5", Qt::CaseInsensitive))
        {
            importType_ = HDF5;
            return;
        }

        if(fnp.endsWith(".watershed", Qt::CaseInsensitive)){
            importType_ = WATERSHED;
            return;
        }

        importType_ = IMAGES;
    }

    void isSourceValid()
    {
        if(files_.empty()){
            throw om::IoException("no source files");
        }

        if(HDF5 == importType_ || WATERSHED == importType_){
            if(1 != files_.size()){
                throw om::IoException("only import one hdf5/watershed file at a time");
            }
        }

        FOR_EACH(iter, files_){
            if(!iter->exists()){
                throw om::IoException("source file not found");
            }
        }
    }
};

