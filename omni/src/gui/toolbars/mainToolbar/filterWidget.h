#ifndef FILTER_WIDGET_H
#define FILTER_WIDGET_H

#include "common/omCommon.h"

#include <QSlider>
#include <boost/optional.hpp>

class MainWindow;

class FilterWidget : public QSlider {
 Q_OBJECT
 public:
	FilterWidget(MainWindow*);

	void increaseAlpha();
	void decreaseAlpha();

 private slots:
	void setFilAlpha(int alpha);

 private:
	OmID getChannelID();
	OmID getFilterID();

	void initSilderTab();
	boost::optional<double> doGetFilterAlpha();
	void doSetFilterAlpha(const double alpha);
	void moveSliderTab(const double alpha);
};

#endif
