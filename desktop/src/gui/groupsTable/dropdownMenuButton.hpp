#pragma once

#include "gui/widgets/omButton.hpp"
#include "gui/groupsTable/groupsTable.h"

class GroupsTableDropDownMenuButton : public OmButton<GroupsTable> {
public:
	GroupsTableDropDownMenuButton(GroupsTable * parent, om::common::SegID seg)
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
	om::common::SegID seg_;

	void doAction()
	{}

};

