#ifndef _HELPER_CHANNEL_HELPER
#define _HELPER_CHANNEL_HELPER

#include "../ui_chanInspector.h"
#include "../chanInspector.h"

#include "inspectorHelper.h"

class ChannelHelper : public InspectorHelper<ChannelDataWrapper> {

 Q_OBJECT

 public:
	ChannelHelper( MyInspectorWidget* parent );
	void addToSplitter( ChannelDataWrapper data );

 private:
	ChanInspector *channelInspectorWidget;
	void populateChannelInspector(OmId c_id);

};

#endif


