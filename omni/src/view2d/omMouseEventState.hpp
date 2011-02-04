#ifndef OM_MOUSE_EVENT_STATE_HPP
#define OM_MOUSE_EVENT_STATE_HPP

class OmMouseEventState {
public:
    template <class T>
    static void SetState(T* target, OmView2dState* state, QMouseEvent* event)
    {
        target->event_ = event;

        target->controlKey_ = om::gui::IsControlDown(event);
        target->shiftKey_ = om::gui::IsShiftDown(event);
        target->altKey_ = om::gui::IsAltDown(event);
        target->leftMouseButton_ = om::gui::IsLeftButton();
        target->rightMouseButton_ = om::gui::IsRightButton();
        target->tool_ = OmStateManager::GetToolMode();
        target->dataClickPoint_ = state->ComputeMouseClickPointDataCoord(event);

        //Print(target);
    }

    template <class T>
    static void Print(T* target)
    {
        printf("controlKey_(%d), altKey_(%d), shiftKey_(%d), leftMouseButton_(%d), "
               "rightMouseButton_(%d), ",
               target->controlKey_,
               target->altKey_,
               target->shiftKey_,
               target->leftMouseButton_,
               target->rightMouseButton_);

        std::cout << target->tool_;
        std::cout << "click point: " << target->dataClickPoint_;
        std::cout << "\n";
    }
};

#endif
