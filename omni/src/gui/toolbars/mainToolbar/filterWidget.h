#ifndef FILTER_WIDGET_H
#define FILTER_WIDGET_H

#include "common/omCommon.h"

#include <QSlider>

class MainWindow;

class FilterWidget : public QSlider {
 Q_OBJECT
 public:
	FilterWidget(MainWindow * parent);
	void updateSilder();

 private slots:
	void setFilAlpha(int alpha);

 private:
	OmId getChannelID();
	OmId getFilterID();
};

#endif
