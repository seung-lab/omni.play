#ifndef OM_GROUP_LIST_WIDGET_H
#define OM_GROUP_LIST_WIDGET_H

#include <QtGui>
#include "common/omCommon.h"
#include "volume/omSegmentation.h"

class GroupsTable;

class OmGroupListWidget : public QTreeWidget {

 Q_OBJECT

 public:
	OmGroupListWidget(GroupsTable * gt);

	void populate(OmSegmentation & seg);

	static string eventSenderName();

 private:
	GroupsTable * mGroupsTable;
	void mousePressEvent(QMouseEvent * event);
	void keyPressEvent(QKeyEvent * event);
	void populateTable();

};

#endif
