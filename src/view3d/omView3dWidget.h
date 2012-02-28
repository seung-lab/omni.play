#pragma once

/*
 * An interface for widgets belonging to the OmView3d viewing system.
 *
 * Can call a View3d event if the widget needs to be redrawn.
 */


class OmCamera;
class OmView3d;

class OmView3dWidget {
private:
    OmView3d *const view3d_;

public:
    OmView3dWidget(OmView3d *view3d)
        : view3d_(view3d)
    {
        enabled = false;
    }

    virtual ~OmView3dWidget()
    {}

    virtual void Draw() = 0;

    OmView3d* View3dPtr() const {
        return view3d_;
    }

    bool enabled;
};

