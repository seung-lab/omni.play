#ifndef OM_BUTTON_H
#define OM_BUTTON_H

#include <QtGui>

template < class T >
class OmButton : public QPushButton {
 public:
	OmButton( T *, const QString &, 
		  const QString &, const bool );

	void setKeyboardShortcut(const QString & shortcut);
	void setIconAndText(const QString & iconPath);

 protected:
	T *const mParent;
	virtual void doAction() = 0;

 private:
	void mousePressEvent(QMouseEvent * event);
};

#endif
