#ifndef OM_CHECK_BOX_WIDGET_HPP
#define OM_CHECK_BOX_WIDGET_HPP

#include <QtGui>

class OmCheckBoxWidget : public QCheckBox {
Q_OBJECT
public:
	OmCheckBoxWidget(QWidget* p, const QString& title)
		: QCheckBox(title, p)
	{
		connect(this, SIGNAL(stateChanged(int)),
				this, SLOT(action(int)));
	}

protected:
	void set(const bool val){
		if(val){
			setCheckState(Qt::Checked);
		} else {
			setCheckState(Qt::Unchecked);
		}
	}

private slots:
    void action(int state){
		if (state == Qt::Checked) {
			doAction(true);
		} else {
			doAction(false);
		}
	}

private:
	virtual void doAction(const bool isChecked) = 0;
};

#endif
