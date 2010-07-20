#include "gui/widgets/omButton.h"
#include "gui/mainwindow.h"

template < class T >
OmButton<T>::OmButton( T * mw, 
		    const QString & title, 
		    const QString & statusTip,
		    const bool isCheckable )
	: QPushButton(mw)
	, mParent(mw)
{
	setText(title);
	setStatusTip(statusTip);
	setCheckable(isCheckable);
}

template < class T >
void OmButton<T>::mousePressEvent(QMouseEvent* event)
{
	QPushButton::mousePressEvent(event);

        if (event->button() == Qt::LeftButton) {
		doAction();
	}
}

template < class T >
void OmButton<T>::setKeyboardShortcut(const QString & shortcut)
{
	setShortcut(QKeySequence(shortcut));
}

template < class T >
void OmButton<T>::setIconAndText(const QString & iconPath)
{
	setIcon(QIcon(iconPath));
	if( !icon().isNull() ){
		setText("");
	}
}
