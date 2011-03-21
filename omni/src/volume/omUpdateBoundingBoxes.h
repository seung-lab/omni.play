#ifndef OM_UPDATE_BOUNDING_BOXES_H
#define OM_UPDATE_BOUNDING_BOXES_H

class OmSegmentation;
class OmSegments;
class OmChunkCoord;

class OmUpdateBoundingBoxes {
private:
    OmSegmentation *const vol_;
    OmSegments *const segments_;

public:
    OmUpdateBoundingBoxes(OmSegmentation* vol);
    void Update();

private:
    void doUpdate(const OmChunkCoord& coord);
};

#endif
