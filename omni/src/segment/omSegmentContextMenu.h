#ifndef OM_SEGMENT_CONTEXT_MENU_H
#define OM_SEGMENT_CONTEXT_MENU_H

#include "common/omCommon.h"
#include <QMenu>

class OmSegmentContextMenu : public QMenu {

	Q_OBJECT
	
public:
	void Refresh(OmId segmentationId, OmId segmentId);
	
	
protected:
	void AddSelectionNames();
	void AddEditSelectionAction();
	void AddSelectionAction();
	void AddDisableActions();
	void AddMergeAction();
	void AddVoxelAction();
	
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
	
private:
	OmId mSegmentationId;
	OmId mSegmentId;
};

#endif
