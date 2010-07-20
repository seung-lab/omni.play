#ifndef OM_IMAGE_DIALOG_H
#define OM_IMAGE_DIALOG_H

#include <QtGui>

class OmImageDialog : public QDialog {
public:
	OmImageDialog(QWidget * p) : QDialog(p)
	{
		imageLabel = new QLabel();
		imageLabel->setBackgroundRole(QPalette::Base);
		imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
		imageLabel->setScaledContents(true);

		layout = new QVBoxLayout(this);
		layout->addWidget(imageLabel);
		
		setWindowTitle(tr("Image Dialog"));
		resize(500, 400);

		show();
 	}
	
	void setPixmap(const QPixmap & p)
	{
		imageLabel->setPixmap(p);
		resize(p.width(), p.height());
	}

private:
	QLabel * imageLabel;
	QScrollArea *scrollArea;
	
	QVBoxLayout* layout;
	
};

#endif
