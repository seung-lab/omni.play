#ifndef FILTER_WIDGET_IMPL_HPP
#define FILTER_WIDGET_IMPL_HPP

#include "common/omCommon.h"
#include "gui/mainwindow.h"
#include "project/omProject.h"
#include "volume/omChannel.h"
#include "volume/omSegmentation.h"
#include "system/omEvents.h"
#include "utility/dataWrappers.h"
#include "viewGroup/omViewGroupState.h"

#include <QSize>
#include <QSlider>
#include <boost/optional.hpp>

class FilterWidgetImpl : public QSlider {
Q_OBJECT
public:

	FilterWidgetImpl()
		: QSlider(Qt::Horizontal)
	{
		QSize size = sizeHint();
		size.setWidth(200);
		setMaximumSize(size);

		initSilderTab();

		connect(this, SIGNAL(valueChanged(int)),
				this, SLOT(setFilAlpha(int)),
				Qt::DirectConnection);
	}

	void IncreaseAlpha()
	{
		const double delta = 0.1;

		const boost::optional<double> alpha = doGetFilterAlpha();
		if(!alpha){
			return;
		}

		double newAlpha = *alpha + delta;

		if(newAlpha > 1){
			newAlpha = 1;
		}

		doSetFilterAlpha(newAlpha);
	}

	void DecreaseAlpha()
	{
		const double delta = 0.1;

		const boost::optional<double> alpha = doGetFilterAlpha();
		if(!alpha){
			return;
		}

		double newAlpha = *alpha - delta;

		if(newAlpha < 0){
			newAlpha = 0;
		}

		doSetFilterAlpha(newAlpha);
	}


	void Cycle()
	{
		const boost::optional<double> alpha = doGetFilterAlpha();
		if(!alpha){
			return;
		}

		double delta = 0.1;
		if(*alpha == 1){
			delta = -0.1;
		}

		double newAlpha = *alpha + delta;

		if(newAlpha < 0){
			newAlpha = 0;
		}
		if(newAlpha > 1){
			newAlpha = 1;
		}

		doSetFilterAlpha(newAlpha);
	}

private slots:
// sliderVal is from 0 to 100
	void setFilAlpha(const int sliderVal)
	{
		const double alpha = sliderVal / 100.0;
		doSetFilterAlpha(alpha);
	}

private:
	OmID getChannelID()
	{
		return 1;
	}

	OmID getFilterID()
	{
		return 1;
	}

	void initSilderTab()
	{
		const boost::optional<double> alpha = doGetFilterAlpha();

		if(alpha){
			moveSliderTab(*alpha);
		}
	}

	void moveSliderTab(const double alpha)
	{
		this->setValue(alpha * 100);
		OmEvents::Redraw2d();
	}

	boost::optional<double> doGetFilterAlpha()
	{
		FilterDataWrapper fdw(getChannelID(), getFilterID());

		if(fdw.isValid()){
			OmFilter2d* filter = fdw.getFilter();
			return boost::optional<double>(filter->GetAlpha());
		}

		return boost::optional<double>();
	}

	void doSetFilterAlpha(const double alpha)
	{
		FilterDataWrapper fdw(getChannelID(), getFilterID());

		if(fdw.isValid()){
			OmFilter2d* filter = fdw.getFilter();
			filter->SetAlpha(alpha);
			moveSliderTab(alpha);
			OmEvents::Redraw2d();
		}
	}

};

#endif
