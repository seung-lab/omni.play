#ifndef OM_ALPHA_VEGA_MODE_HPP
#define OM_ALPHA_VEGA_MODE_HPP

#include "common/omDebug.h"
#include "system/events/omView3dEvent.h"
#include "gui/toolbars/mainToolbar/filterWidget.h"

#include <qthread.h>

class OmAlphaVegaMode: public QThread {
public:
	OmAlphaVegaMode()
		: stop_(false)
	{
		start();
	}

	~OmAlphaVegaMode()
	{
		stop_ = true;
		wait();
	}

	void run()
	{
		forever{
			if(stop_){
				return;
			}

			FilterWidget::Cycle();
			yieldCurrentThread();
		}
	}

private:
	bool stop_;
};

#endif
