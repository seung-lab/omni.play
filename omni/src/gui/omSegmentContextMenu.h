#ifndef OM_SEGMENT_CONTEXT_MENU_H
#define OM_SEGMENT_CONTEXT_MENU_H

#include "common/omCommon.h"
#include <QMenu>

class OmViewGroupState;
class OmSegmentContextMenu : public QMenu {

	Q_OBJECT
	
public:
	void Refresh(OmId segmentationId, OmId segmentId, OmViewGroupState * vgs);
	
	
protected:
	void AddSelectionNames();
	void AddEditSelectionAction();
	void AddSelectionAction();
	void AddDisableActions();
	void AddDendActions();
	void AddVoxelAction();
	void AddColorActions();
	void AddGroupActions();

	
private slots:
	void Select();
	void Unselect();
	void UnselectOthers();
	
	void Disable();
	void Enable();
	void DisableSelected();
	void DisableUnselected();
	
	void SetEditSelection();
	void MergeSegments();
	void SetConnectedVoxels();
	void splitSegments();
	void randomizeColor();
	void addGroup();
	void deleteGroup();
	
private:
	OmId mSegmentationId;
	OmId mSegmentId;
	OmViewGroupState * mViewGroupState;
};

#endif
