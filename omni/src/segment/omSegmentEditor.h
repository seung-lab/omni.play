#ifndef OM_SEGMENT_EDITOR_H
#define OM_SEGMENT_EDITOR_H

/*
 *
 */

#include "volume/omVolumeTypes.h"
#include "system/omSystemTypes.h"
#include "common/omStd.h"

class OmSegmentEditor {

 public:
	static OmSegmentEditor *Instance();
	static void Delete();

	//edit selection
	static void SetEditSelection(OmId segmentation, OmId segment);
	static bool GetEditSelection(OmId & segmentation, OmId & segment);

	//voxel selection
	static void SetSelectedVoxels(const set < DataCoord > &voxels);
	static const set < DataCoord > &GetSelectedVoxels();
	static void ClearSelectedVoxels();

	static void SetSelectedVoxelState(const DataCoord & dataCoord, bool selected);
	static bool GetSelectedVoxelState(const DataCoord & dataCoord);

	//voxel
	static DataCoord SpaceCoordToEditSelectionVoxel(const SpaceCoord & spaceCoord);
	static void DrawEditSelectionVoxels();

 protected:
	// singleton constructor, copy constructor, assignment operator protected
	 OmSegmentEditor();
	~OmSegmentEditor();
	 OmSegmentEditor(const OmSegmentEditor &);
	 OmSegmentEditor & operator=(const OmSegmentEditor &);

 private:
	//singleton
	static OmSegmentEditor *mspInstance;

	OmId mEditSegmentation, mEditSegment;
	 set < DataCoord > mSelectedVoxels;

};

#endif
