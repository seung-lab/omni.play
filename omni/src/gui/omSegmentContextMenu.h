#ifndef OM_SEGMENT_CONTEXT_MENU_H
#define OM_SEGMENT_CONTEXT_MENU_H

#include "common/omCommon.h"
#include <QMenu>

class SegmentDataWrapper;
class OmViewGroupState;

class OmSegmentContextMenu : public QMenu {

	Q_OBJECT
	
public:
	void Refresh( SegmentDataWrapper sdw, OmViewGroupState * vgs);
	
protected:
	void AddSelectionNames();
	void AddEditSelectionAction();
	void AddSelectionAction();
	void AddDendActions();
	void AddVoxelAction();
	void AddColorActions();
	void AddGroupActions();
	void AddPropertiesActions();

private slots:
	void Select();
	void Unselect();
	void UnselectOthers();
	
	void SetEditSelection();
	void MergeSegments();
	void SetConnectedVoxels();
	void splitSegments();
	void randomizeColor();
	void addGroup();
	void deleteGroup();
	void showProperties();
	
private:
	OmId mSegmentationId;
	OmId mSegmentId;
	OmViewGroupState * mViewGroupState;
};

#endif
