#ifndef OM_KEY_PRESS_LISTENER_H
#define OM_KEY_PRESS_LISTENER_H

/*
 * Listener for key events.
 *
 *      Matt Wimer <mwimer@mit.edu> 2010
 */

#include "segment/omSegmentSelected.hpp"
#include "events/omEvent.h"
#include "common/omStd.h"

class OmKeyPressEventListener {
public:
    OmKeyPressEventListener() {}

protected:
    void keyPressEvent(QKeyEvent * event) {
        switch (event->key()) {
        case Qt::Key_Up:
            PressUp(event);
            break;
        case Qt::Key_Down:
            PressDown(event);
            break;

        case Qt::Key_J:
            PressJ(event);
            break;
        case Qt::Key_R:
            OmSegmentSelected::RandomizeColor();
            break;
        case Qt::Key_V:
            PressV(event);
            break;

        default:
            break;
        }
    }

    virtual void PressJ(QKeyEvent*)
    {}

    virtual void PressV(QKeyEvent*) {
        OmSegmentSelected::ToggleValid();
    }

    virtual void PressDown(QKeyEvent*)
    {}

    virtual void PressUp(QKeyEvent*)
    {}
};

#endif
