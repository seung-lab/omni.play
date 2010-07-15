#ifndef OM_APPLY_EDIT_SELECTION_ACTION_H
#define OM_APPLY_EDIT_SELECTION_ACTION_H

/*
 *	Apply the edit selection to the set of selected voxels.
 *
 */


#include "common/omStd.h"


class OmEditSelectionApplyAction {

public:
	OmEditSelectionApplyAction();

private:
	void Action();
	void UndoAction();
	string Description();
	void save(const string &);
	QString classNameForLogFile(){return "OmEditSelectionApplyAction";}

	OmId mSegmentationId, mSegmentId;
	map< DataCoord, OmId > mVoxelMap;

	friend class OmActionLoggerFS;
};

#endif
