#ifndef FILOBJECTINSPECTOR_H
#define FILOBJECTINSPECTOR_H

#include <QWidget>
#include "ui_filObjectInspector.h"
#include "system/omSystemTypes.h"

class FilObjectInspector:public QWidget, public Ui::filObjectInspector {
 Q_OBJECT public:
	FilObjectInspector(QWidget * parent = 0);

	void setChannelID(const OmId);
	OmId getChannelID();
	void setFilterID(const OmId);
	OmId getFilterID();

 private:
	OmId ChannelID;
	OmId FilterID;
};
#endif
