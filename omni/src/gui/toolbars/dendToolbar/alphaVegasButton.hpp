#ifndef ALPHA_VEGAS_BUTTON_HPP
#define ALPHA_VEGAS_BUTTON_HPP

#include "gui/widgets/omButton.h"
#include "system/omAlphaVegasMode.hpp"

class AlphaVegasButton : public OmButton<DisplayTools> {
 public:
	AlphaVegasButton(DisplayTools *dt)
	: OmButton<DisplayTools>(dt,
							"Alpha Vegas Mode",
							"cycle alpha slider",
							true)
	{}

 private:
	boost::shared_ptr<OmAlphaVegaMode> vegas_;

	void doAction()
	{
		if(vegas_){
			vegas_.reset();
		} else {
			vegas_ = boost::make_shared<OmAlphaVegaMode>();
		}
	}
};

#endif
