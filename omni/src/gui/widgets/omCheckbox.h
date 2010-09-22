#ifndef OM_CHECKBOX_H
#define OM_CHECKBOX_H

#include <QtGui>

template < class T >
class OmCheckBox : public QCheckBox {
public:
	OmCheckBox(T* parent, const QString & title)
		: QCheckBox(parent)
		, mParent(parent)
	{
		setText(title);
	}

protected:
	T *const mParent;
	virtual void doAction() = 0;

private:
	void mousePressEvent(QMouseEvent * event)
	{
		QCheckBox::mousePressEvent(event);

		doAction();
	}
};

#endif

