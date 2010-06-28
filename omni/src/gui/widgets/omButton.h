#ifndef OM_BUTTON_H
#define OM_BUTTON_H

#include <QtGui>
class MainWindow;

class OmButton : public QPushButton {
 public:
	OmButton( MainWindow *, const QString &, 
		    const QString &, const bool );

 protected:
	MainWindow *const mMainWindow;
	virtual void doAction() = 0;

 private:
	void mousePressEvent(QMouseEvent * event);
};

#endif
