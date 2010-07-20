#ifndef OM_CHECKBOX_H
#define OM_CHECKBOX_H

#include <QtGui>

template < class T >
class OmCheckBox : public QCheckBox {
 public: 
	OmCheckBox( T*, const QString & );

 protected:
	T *const mParent;
	virtual void doAction() = 0;

 private:
	void mousePressEvent(QMouseEvent * event);
};

#endif
