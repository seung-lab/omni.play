#ifndef OM_KEY_PRESS_LISTENER_H
#define OM_KEY_PRESS_LISTENER_H

/*
 *	Listener for key events.
 *
 *      Matt Wimer <mwimer@mit.edu> 2010
 */

#include "segment/omSegmentSelected.hpp"
#include "system/events/omEvent.h"
#include "common/omStd.h"

/*
 *      View Event
 */
class OmKeyPressEventListener {
public:
	OmKeyPressEventListener() {}

protected:
	void keyPressEvent(QKeyEvent * event) {
		switch (event->key()) {
		case Qt::Key_V:
			PressV(event);
			break;
		case Qt::Key_Up:
			PressUp(event);
			break;
		case Qt::Key_Down:
			PressDown(event);
			break;
		case Qt::Key_R:
			OmSegmentSelected::RandomizeColor();
			break;
		default:
			break;
		}
	}

	virtual void PressV(QKeyEvent * ) {
		OmSegmentSelected::ToggleValid();
	}

	virtual void PressDown(QKeyEvent*){}
	virtual void PressUp(QKeyEvent*){}
};

#endif
