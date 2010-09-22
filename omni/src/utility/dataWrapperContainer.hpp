#ifndef DATA_WRAPPER_CONTAINER_HPP
#define DATA_WRAPPER_CONTAINER_HPP

#include <boost/optional.hpp>

class DataWrapperContainer {
private:
	boost::optional<SegmentationDataWrapper> segmenDW;
	boost::optional<ChannelDataWrapper> cDW;
	ObjectType mType;

public:
	DataWrapperContainer(){}

	DataWrapperContainer(const ObjectType obj_type, const OmId obj_id)
		: mType(obj_type)
	{
		switch (obj_type) {
		case CHANNEL:
			cDW = boost::optional<ChannelDataWrapper>(obj_id);
			break;
		case SEGMENTATION:
			segmenDW = boost::optional<SegmentationDataWrapper>(obj_id);
			break;
		}
	}

	explicit DataWrapperContainer(SegmentationDataWrapper sdw)
	{
		mType = SEGMENTATION;
		segmenDW = boost::optional<SegmentationDataWrapper>(sdw);
	}

	bool isValidContainer() const {
		return segmenDW || cDW;
	}

	ObjectType getType() const {
		return mType;
	}
	SegmentationDataWrapper getSegmentationDataWrapper() const {
		return *segmenDW;
	}
	ChannelDataWrapper getChannelDataWrapper() const {
		return *cDW;
	}
};

#endif
