#ifndef OM_GUI_HELPERS_HPP
#define OM_GUI_HELPERS_HPP

namespace om {
namespace gui {

inline bool IsButton(QMouseEvent* event, const Qt::MouseButton button)
{
    return QApplication::mouseButtons() & button ||
        event->buttons() & button;
}

inline bool IsLeftButton(QMouseEvent* event){
    return IsButton(event, Qt::LeftButton);
}

inline bool IsRightButton(QMouseEvent* event){
    return IsButton(event, Qt::RightButton);
}

inline bool IsKeyDown(QMouseEvent* event, const Qt::KeyboardModifier key)
{
    return QApplication::keyboardModifiers() & key ||
        event->modifiers() & key;
}

inline bool NoModifer(QMouseEvent* event){
    return IsKeyDown(event, Qt::NoModifier);
}

inline bool IsControlDown(QMouseEvent* event){
    return IsKeyDown(event, Qt::ControlModifier);
}

inline bool IsShiftDown(QMouseEvent* event){
    return IsKeyDown(event, Qt::ShiftModifier);
}

inline bool IsAltDown(QMouseEvent* event){
    return IsKeyDown(event, Qt::AltModifier);
}

}
}

#endif
