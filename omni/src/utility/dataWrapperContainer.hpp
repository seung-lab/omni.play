#ifndef DATA_WRAPPER_CONTAINER_HPP
#define DATA_WRAPPER_CONTAINER_HPP

#include <boost/optional.hpp>

class DataWrapperContainer {
private:
	boost::optional<SegmentationDataWrapper> sdw_;
	boost::optional<ChannelDataWrapper> cdw_;

public:
	DataWrapperContainer(){}

	DataWrapperContainer(const ObjectType obj_type, const OmId obj_id)
	{
		switch (obj_type){
		case CHANNEL:
			cdw_ = boost::optional<ChannelDataWrapper>(obj_id);
			break;
		case SEGMENTATION:
			sdw_ = boost::optional<SegmentationDataWrapper>(obj_id);
			break;
		default:
			throw OmArgException("unknown type");
		}
	}

	explicit DataWrapperContainer(SegmentationDataWrapper sdw)
	{
		sdw_ = boost::optional<SegmentationDataWrapper>(sdw);
	}

	bool isValidContainer() const {
		return sdw_ || cdw_;
	}

	bool isSegmentation() const {
		return sdw_;
	}

	bool isChannel() const {
		return cdw_;
	}

	ObjectType getType() const
	{
		if(cdw_){
			return CHANNEL;
		}
		if(sdw_){
			return SEGMENTATION;
		}
		throw OmArgException("uninitialized");
	}

	SegmentationDataWrapper getSegmentationDataWrapper() const {
		return *sdw_;
	}

	ChannelDataWrapper getChannelDataWrapper() const {
		return *cdw_;
	}
};

#endif
