#include "utility/channelDataWrapper.hpp"
#include "utility/dataWrappers.h"

QHash<OmId, FilterDataWrapper> ChannelDataWrapper::getAllFilterIDsAndNames()
{
	QHash<OmId, FilterDataWrapper> filters;

	OmChannel & chann = OmProject::GetChannel(mID);

	foreach(OmId filterID, chann.GetValidFilterIds()) {
		FilterDataWrapper filter(mID, filterID);
		filters[filterID] = filter;
	}

	return filters;
}
