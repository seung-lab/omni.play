#include "gui/toolbars/mainToolbar/filterWidget.h"
#include "gui/mainwindow.h"
#include "project/omProject.h"
#include "volume/omChannel.h"
#include "volume/omSegmentation.h"
#include "system/omEvents.h"
#include "utility/dataWrappers.h"
#include "system/viewGroup/omViewGroupState.h"

#include <QSize>

FilterWidget::FilterWidget(MainWindow * mw)
	: QSlider(Qt::Horizontal, mw)
{
	QSize size = sizeHint();
	size.setWidth(200);
	setMaximumSize(size);

	initSilderTab();

	connect(this, SIGNAL(valueChanged(int)),
		this, SLOT(setFilAlpha(int)),
		Qt::DirectConnection);

	mw->getViewGroupState()->SetFilterWidget(this);
}

OmId FilterWidget::getChannelID()
{
	return 1;
}

OmId FilterWidget::getFilterID()
{
	return 1;
}

// sliderVal is from 0 to 100
void FilterWidget::setFilAlpha(const int sliderVal)
{
	const double alpha = sliderVal / 100.0;
	doSetFilterAlpha(alpha);
}

void FilterWidget::initSilderTab()
{
	const boost::optional<double> alpha = doGetFilterAlpha();
	if(!alpha){
		return;
	}

	moveSliderTab(*alpha);
}

void FilterWidget::moveSliderTab(const double alpha)
{
	this->setValue(alpha * 100);
	OmEvents::Redraw();
}

boost::optional<double> FilterWidget::doGetFilterAlpha()
{
	FilterDataWrapper fdw(getChannelID(), getFilterID());

	if(fdw.isValid()){
		OmFilter2d* filter = fdw.getFilter();
		return boost::optional<double>(filter->GetAlpha());
	}

	return boost::optional<double>();
}

void FilterWidget::doSetFilterAlpha(const double alpha)
{
	FilterDataWrapper fdw(getChannelID(), getFilterID());

	if(fdw.isValid()){
		OmFilter2d * filter = fdw.getFilter();
		filter->SetAlpha(alpha);
		moveSliderTab(alpha);
		OmEvents::Redraw();
	}
}

void FilterWidget::increaseAlpha()
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

void FilterWidget::decreaseAlpha()
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
