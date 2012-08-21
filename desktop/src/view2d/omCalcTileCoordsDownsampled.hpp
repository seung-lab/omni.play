#pragma once

#include "tiles/cache/omTileCache.h"
#include "tiles/omTileTypes.hpp"
#include "utility/dataWrappers.h"
#include "view2d/omView2dConverters.hpp"
#include "zi/omUtility.h"

// class OmCalcTileCoordsDownsampled {
// private:
//     const ViewType viewType_;

// public:
//     OmCalcTileCoordsDownsampled(const ViewType viewType)
//         : viewType_(viewType)
//     {}

//     void TryDownsample(const OmTileCoordAndVertices& tcv,
//                        std::deque<OmTileAndVertices>& tilesToDraw)
//     {
//         OmMipVolume* vol = tcv.tileCoord.getVolume();
//         const int rootMipLevel = vol->Coords().GetRootMipLevel();
//         const int initialMipLevel = tcv.tileCoord.getLevel();

//         if(initialMipLevel == rootMipLevel){
//             return;
//         }

//         const OmTileCoord& initialTileCoord = tcv.tileCoord;

//         for(int mipLevel = initialMipLevel + 1; mipLevel <= rootMipLevel; ++mipLevel)
//         {
//             const OmTileCoord downsampledTileCoord = makeNewTileCoord(initialTileCoord, mipLevel);

//             OmTilePtr downsampledTile;
//             OmTileCache::GetDontQueue(downsampledTile, downsampledTileCoord);

//             if(downsampledTile)
//             {
//                 addTile(downsampledTile, mipLevel, tcv, tilesToDraw);
//                 return;
//             }
//         }

//         // just queue up downsampled tile
//         int mipLevel = initialMipLevel + 1;

//         OmTileCache::QueueUp(makeNewTileCoord(initialTileCoord, mipLevel));

//         if(rootMipLevel != mipLevel){
//             OmTileCache::QueueUp(makeNewTileCoord(initialTileCoord, rootMipLevel));
//         }
//     }

//     void addTile(OmTilePtr& downsampledTile, const int mipLevel,
//                  const OmTileCoordAndVertices& tcv, std::deque<OmTileAndVertices>& tilesToDraw)
//     {
//         const int initialMipLevel = tcv.tileCoord.getLevel();
//         const om::globalCoord& initialCoord = tcv.tileCoord.getCoord();
//         const GLfloatBox& vertices = tcv.vertices;

//         OmTileAndVertices tv = {downsampledTile,
//                                 vertices,
//                                 getTextureVertices(initialCoord, initialMipLevel,
//                                                    mipLevel)
//         };

//         tilesToDraw.push_back(tv);
//     }

//     OmTileCoord makeNewTileCoord(const OmTileCoord& initialTileCoord, const int mipLevel)
//     {
//         return OmTileCoord(mipLevel,
//                            makeNewCoord(initialTileCoord.getCoord(), mipLevel),
//                            initialTileCoord.getVolume(),
//                            initialTileCoord.getFreshness(),
//                            initialTileCoord.getViewGroupState(),
//                            initialTileCoord.getViewType(),
//                            initialTileCoord.getSegmentColorCacheType());
//     }

//     om::globalCoord makeNewCoord(const om::globalCoord& oldCoord, const int mipLevel)
//     {
//         const int newTileSize = 128 * om::math::pow2int(mipLevel); // TODO: Don't hardcode 128

//         const Vector2i ptsInPlane = OmView2dConverters::Get2PtsInPlane(oldCoord, viewType_);

//         const float depth = OmView2dConverters::GetViewTypeDepth(oldCoord, viewType_);

//         return OmView2dConverters::MakeViewTypeVector3((float)om::math::roundDown(ptsInPlane.x, newTileSize),
//                                                        (float)om::math::roundDown(ptsInPlane.y, newTileSize),
//                                                        depth,
//                                                        viewType_);
//     }

//     TextureVectices getTextureVertices(const om::globalCoord& oldDC, const int initialMipLevel,
//                                        const int curMipLevel)
//     {
//         const Vector2i initialPtsInPlane = OmView2dConverters::Get2PtsInPlane(oldDC, viewType_);
//         const int initialTileSize = 128 * om::math::pow2int(initialMipLevel);
//         const int finalTileSize = 128 * om::math::pow2int(curMipLevel);

//         const Vector2f pts(initialPtsInPlane.x % finalTileSize,
//                            initialPtsInPlane.y % finalTileSize);

//         const Vector2f textureRet = pts / static_cast<float>(finalTileSize);

//         const float inc = static_cast<float>(initialTileSize) / static_cast<float>(finalTileSize);

//         TextureVectices ret;

//         ret.upperLeft.x = textureRet.x;
//         ret.upperLeft.y = textureRet.y + inc;
//         ret.lowerRight.x = textureRet.x + inc;
//         ret.lowerRight.y = textureRet.y;

//         return ret;
//     }
// };

