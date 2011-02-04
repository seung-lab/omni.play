#ifndef OM_GUI_HELPERS_HPP
#define OM_GUI_HELPERS_HPP

namespace om {
namespace gui {

	bool IsLeftButton(){
		return QApplication::mouseButtons() == Qt::LeftButton;
	}

	bool IsRightButton(){
		return QApplication::mouseButtons() == Qt::RightButton;
	}

	bool NoModifer(QMouseEvent* event){
		return event->modifiers() & Qt::NoModifier;
	}

	bool IsControlDown(QMouseEvent* event){
		return event->modifiers() & Qt::ControlModifier;
	}

	bool IsShiftDown(QMouseEvent* event){
		return event->modifiers() & Qt::ShiftModifier;
	}

	bool IsAltDown(QMouseEvent* event){
		return event->modifiers() & Qt::AltModifier;
	}
}
}

#endif
