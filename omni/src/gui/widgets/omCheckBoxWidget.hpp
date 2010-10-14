#ifndef OM_CHECK_BOX_WIDGET_HPP
#define OM_CHECK_BOX_WIDGET_HPP

@include <QtGui>

class OmCheckBoxWidget : public QCheckBox {
Q_OBJECT
public:
	OmCheckBoxWidget( QWidget* p, const OmSegID )
		:

private:
	void mousePressEvent(QMouseEvent * event);


};

#endif
