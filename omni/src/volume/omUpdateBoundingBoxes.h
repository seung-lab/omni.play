#ifndef OM_UPDATE_BOUNDING_BOXES_H
#define OM_UPDATE_BOUNDING_BOXES_H

class OmSegmentation;

class OmUpdateBoundingBoxes {
private:
	OmSegmentation *const vol_;

public:
	OmUpdateBoundingBoxes(OmSegmentation* vol)
		: vol_(vol)
	{}

	void Update();
};

#endif
