#include "utility/channelDataWrapper.hpp"
#include "utility/dataWrappers.h"

QHash<OmID, FilterDataWrapper> ChannelDataWrapper::getAllFilterIDsAndNames()
{
	QHash<OmID, FilterDataWrapper> filters;

	OmChannel & chann = OmProject::GetChannel(mID);

	foreach(OmID filterID, chann.GetValidFilterIds()) {
		FilterDataWrapper filter(mID, filterID);
		filters[filterID] = filter;
	}

	return filters;
}
