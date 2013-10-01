#pragma once

#include "events/listeners.h"

class OmTileCacheImpl;

class OmTileCacheEventListener : public om::event::View2dEventListener {
 private:
  OmTileCacheImpl* const cache_;

  virtual void ViewBoxChangeEvent() {}
  virtual void ViewPosChangeEvent() {}
  virtual void ViewRedrawEvent() {}
  virtual void ViewBlockingRedrawEvent() {}
  virtual void CoordSystemChangeEvent() {}
  virtual void ViewCenterChangeEvent();

 public:
  OmTileCacheEventListener(OmTileCacheImpl* cache);
};
