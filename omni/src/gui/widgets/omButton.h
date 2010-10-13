#ifndef OM_BUTTON_H
#define OM_BUTTON_H

#include <QtGui>

template < class T >
class OmButton : public QPushButton {
public:
	OmButton( T * mw,
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

	void setKeyboardShortcut(const QString & shortcut){
		setShortcut(QKeySequence(shortcut));
	}

	void setIconAndText(const QString & iconPath)
	{
		setIcon(QIcon(iconPath));
		if( !icon().isNull() ){
			setText("");
		}
	}

protected:
	T *const mParent;
	virtual void doAction() = 0;

private:
	void mousePressEvent(QMouseEvent* event)
	{
		QPushButton::mousePressEvent(event);

        if (event->button() == Qt::LeftButton) {
			doAction();
		}
	}
};

#endif
