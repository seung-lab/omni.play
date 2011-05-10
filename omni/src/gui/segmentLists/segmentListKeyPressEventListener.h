#pragma once

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

