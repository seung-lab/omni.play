#pragma once

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

