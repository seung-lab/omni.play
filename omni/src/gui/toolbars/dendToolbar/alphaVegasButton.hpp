#ifndef ALPHA_VEGAS_BUTTON_HPP
#define ALPHA_VEGAS_BUTTON_HPP

#include "gui/widgets/omButton.hpp"
#include "system/omAlphaVegasMode.hpp"
#include "gui/toolbars/dendToolbar/displayTools/displayTools.h"

class AlphaVegasButton : public OmButton<DisplayTools> {
 public:
	AlphaVegasButton(DisplayTools *dt)
	: OmButton<DisplayTools>(dt,
							"Vegas Mode",
							"cycle alpha slider",
							true)
	{}

 private:
	boost::shared_ptr<OmAlphaVegasMode> vegas_;

	void doAction()
	{
		if(vegas_){
			vegas_.reset();
		} else {
			vegas_ = boost::make_shared<OmAlphaVegasMode>();
		}
	}
};

#endif
