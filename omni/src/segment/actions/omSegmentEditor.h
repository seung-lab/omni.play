#ifndef OM_SEGMENT_EDITOR_H
#define OM_SEGMENT_EDITOR_H

#include "common/omCommon.h"

class SegmentDataWrapper;

class OmSegmentEditor : boost::noncopyable {

public:
	static OmSegmentEditor* Instance();
	static void Delete();

	//edit selection
	static void SetEditSelection(OmId segmentation, OmId segment);
	static SegmentDataWrapper GetEditSelection();

	//voxel selection
	static void SetSelectedVoxels(const std::set<DataCoord> &voxels);
	static const std::set<DataCoord>& GetSelectedVoxels();
	static void ClearSelectedVoxels();

	static void SetSelectedVoxelState(const DataCoord &dataCoord, bool selected);
	static bool GetSelectedVoxelState(const DataCoord &dataCoord);

private:
	OmSegmentEditor();
	~OmSegmentEditor();

	//singleton
	static OmSegmentEditor* mspInstance;

	OmId mEditSegmentation, mEditSegment;
	std::set<DataCoord> mSelectedVoxels;

};

#endif
