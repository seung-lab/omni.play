#ifndef OM_SEGMENT_CONTEXT_MENU_H
#define OM_SEGMENT_CONTEXT_MENU_H

#include "common/omCommon.h"
#include "utility/dataWrappers.h"

#include <QMenu>

class OmViewGroupState;

class OmSegmentContextMenu : public QMenu {

Q_OBJECT

public:
	void Refresh(const SegmentDataWrapper& sdw, OmViewGroupState* vgs);

protected:
	void addSelectionNames();
	void addSelectionAction();
	void addDendActions();
	void addColorActions();
	void addGroupActions();
	void addPropertiesActions();

private slots:
	void select();
	void unselect();
	void unselectOthers();

	void mergeSegments();
	void splitSegments();
	void randomizeColor();
	void setValid();
	void setNotValid();
	void showProperties();
	void printChildren();

private:
	SegmentDataWrapper sdw_;

	bool isValid() const;
	bool isUncertain() const;

	OmViewGroupState * mViewGroupState;
};

#endif
