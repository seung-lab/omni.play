#ifndef FILTER_WIDGET_HPP
#define FILTER_WIDGET_HPP

#include "gui/toolbars/mainToolbar/filterWidgetImpl.hpp"
#include "zi/omUtility.h"

class FilterWidget : private om::singletonBase<FilterWidget>{
private:
	FilterWidgetImpl* impl_;

public:
	static QWidget* Widget(){
		return instance().impl_;
	}

	static void Create(){
		instance().impl_ = new FilterWidgetImpl();
	}

	static void IncreaseAlpha(){
		instance().impl_->IncreaseAlpha();
	}

	static void DecreaseAlpha(){
		instance().impl_->DecreaseAlpha();
	}

private:
	FilterWidget()
		: impl_(NULL)
	{}
	~FilterWidget(){}

	friend class zi::singleton<FilterWidget>;
};

#endif
