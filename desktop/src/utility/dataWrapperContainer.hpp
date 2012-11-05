#pragma once

#include "common/common.h"
#include "utility/channelDataWrapper.hpp"
#include "utility/segmentationDataWrapper.hpp"
#include "utility/affinityGraphDataWrapper.hpp"
#include <boost/optional.hpp>

class DataWrapperContainer {
private:
    boost::optional<SegmentationDataWrapper> sdw_;
    boost::optional<ChannelDataWrapper> cdw_;
    boost::optional<AffinityGraphDataWrapper> adw_;

public:
    DataWrapperContainer(){}

    DataWrapperContainer(const ObjectType obj_type, const om::common::ID obj_id)
    {
        switch (obj_type){
        case CHANNEL:
            cdw_ = boost::optional<ChannelDataWrapper>(obj_id);
            break;
        case SEGMENTATION:
            sdw_ = boost::optional<SegmentationDataWrapper>(obj_id);
            break;
        case AFFINITY:
            adw_ = boost::optional<AffinityGraphDataWrapper>(obj_id);
            break;
        default:
            throw ArgException("unknown type");
        }
    }

    explicit DataWrapperContainer(SegmentationDataWrapper sdw)
    {
        sdw_ = boost::optional<SegmentationDataWrapper>(sdw);
    }

    bool isValidContainer() const {
        return sdw_ || cdw_ || adw_;
    }

    bool isSegmentation() const {
        return sdw_;
    }

    bool isChannel() const {
        return cdw_;
    }
    
    bool isAffinityGraph() const {
        return adw_;
    }

    ObjectType getType() const
    {
        if(cdw_){
            return CHANNEL;
        }
        
        if(sdw_){
            return SEGMENTATION;
        }
        
        if(adw_){
            return AFFINITY;
        }
        throw ArgException("uninitialized");
    }

    SegmentationDataWrapper GetSDW() const {
        return *sdw_;
    }

    ChannelDataWrapper getChannelDataWrapper() const {
        return *cdw_;
    }
    
    AffinityGraphDataWrapper GetADW() const {
        return *adw_;
    }   
};

