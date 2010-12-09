#ifndef GROUPS_TABLE_DROP_DOWN_MENU_BUTTON_HPP
#define GROUPS_TABLE_DROP_DOWN_MENU_BUTTON_HPP

#include "gui/widgets/omButton.hpp"
#include "gui/groupsTable/groupsTable.h"

class GroupsTableDropDownMenuButton : public OmButton<GroupsTable> {
public:
	GroupsTableDropDownMenuButton(GroupsTable * parent, OmSegID seg)
		: OmButton<GroupsTable>(parent, QString("%1").arg(seg), "", false)
		, seg_(seg)
	{}

protected:
	virtual bool event(QEvent * event)
	{
		mParent->SetSegmentID(seg_);
		return QWidget::event(event);
	}

private:
	OmSegID seg_;

	void doAction()
	{}

};

#endif
