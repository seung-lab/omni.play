#pragma once

#include "events/details/omViewEvent.h"

class OmTileCacheImpl;

class OmTileCacheEventListener : public OmViewEventListener {
private:
    OmTileCacheImpl *const cache_;

    virtual void ViewBoxChangeEvent() {}
    virtual void ViewPosChangeEvent() {}
    virtual void ViewRedrawEvent() {}
    virtual void ViewBlockingRedrawEvent() {}
    virtual void AbsOffsetChangeEvent() {}
    virtual void ViewCenterChangeEvent();

public:
    OmTileCacheEventListener(OmTileCacheImpl* cache);
};

