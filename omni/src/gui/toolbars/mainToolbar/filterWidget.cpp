#include "gui/toolbars/mainToolbar/filterWidget.h"
#include "gui/mainwindow.h"
#include "project/omProject.h"
#include "volume/omChannel.h"
#include "volume/omSegmentation.h"
#include "system/omEvents.h"
#include "utility/dataWrappers.h"

#include <QSize>

FilterWidget::FilterWidget(MainWindow * mw)
	: QSlider(Qt::Horizontal, mw)
{
	QSize size = sizeHint();
	size.setWidth(200);
	setMaximumSize(size);

	updateSilder();

	connect(this, SIGNAL(valueChanged(int)), 
		this, SLOT(setFilAlpha(int)), 
		Qt::DirectConnection);
}

OmId FilterWidget::getChannelID()
{
	return 1;
}

OmId FilterWidget::getFilterID()
{
	return 1;
}

void FilterWidget::setFilAlpha(int alpha)
{
	FilterDataWrapper fdw(getChannelID(), getFilterID());

	if(fdw.isValid()){
		OmFilter2d * filter = fdw.getFilter();
		filter->SetAlpha((double)alpha / 100.0 );
		OmEvents::Redraw();
	}
}

void FilterWidget::updateSilder()
{
	FilterDataWrapper fdw(getChannelID(), getFilterID());

	if(fdw.isValid()){
		OmFilter2d * filter = fdw.getFilter();
		this->setValue(filter->GetAlpha() * 100);
		OmEvents::Redraw();
	}
}
