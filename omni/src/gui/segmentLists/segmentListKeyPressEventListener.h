#ifndef OM_SEGMENT_LIST_KEY_PRESS_EVENT_LISTENER_H
#define OM_SEGMENT_LIST_KEY_PRESS_EVENT_LISTENER_H

#include "segment/omSegmentSelected.hpp"

class SegmentListKeyPressEventListener {
public:
    SegmentListKeyPressEventListener()
    {}

    void keyPressEvent(QKeyEvent* event)
    {
        switch (event->key()) {
        case Qt::Key_Down:
            printf("segment list key down\n");
            break;
        case Qt::Key_Up:
            printf("segment list key up\n");
            break;
        }
    }
};

#endif
