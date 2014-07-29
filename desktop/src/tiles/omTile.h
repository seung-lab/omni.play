#pragma once
#include "precomp.h"

/*
 *  OmTile allows access to 2D image data from the source volume.
 *
 *  Rachel Shearer - rshearer@mit.edu
 */

#include "tiles/omTileCoord.h"
#include "coordinates/tile.h"

class OmTextureID;
class OmMipVolume;
class OmCacheBase;
class OmViewGroupState;

class OmTile {
 public:
  OmTile(OmCacheBase* cache, const OmTileCoord& key);
  ~OmTile();

  void LoadData();
  uint32_t NumBytes() const;

  inline OmTextureID* GetTexture() { return texture_.get(); }

  inline const OmTileCoord& GetTileCoord() const { return key_; }

 private:
  OmCacheBase* const cache_;
  const OmTileCoord key_;
  const om::coords::Tile coord_;
  const int tileLength_;
  const om::coords::Chunk mipChunkCoord_;

  std::unique_ptr<OmTextureID> texture_;

  void load8bitChannelTile();
  void load32bitSegmentationTile();

  om::coords::Chunk tileToMipCoord();
  int getDepth();
  void setVertices(const int x, const int y, const float zoomFactor);

  inline OmMipVolume& getVol() const { return key_.getVolume(); }

  om::common::ObjectType getVolType() const;
};
