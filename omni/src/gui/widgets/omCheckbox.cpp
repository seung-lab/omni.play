#include "gui/widgets/omCheckbox.h"

template < class T >
OmCheckBox<T>::OmCheckBox(T* parent, const QString & title)
	: QCheckBox(parent)
	, mParent(parent)
{
	setText(title);
}

template < class T >
void OmCheckBox<T>::mousePressEvent(QMouseEvent* event)
{
	QCheckBox::mousePressEvent(event);

	doAction();
}
