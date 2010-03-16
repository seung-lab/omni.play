#include <QtGui>
#include "guiUtils.h"
#include "localPreferences3d.h"
#include "common/omDebug.h"
#include "system/omLocalPreferences.h"

LocalPreferences3d::LocalPreferences3d(QWidget * parent)
 : QWidget(parent)
{
	QVBoxLayout* overallContainer = new QVBoxLayout( this );

	overallContainer->addWidget( makeGeneralPropBox());
	overallContainer->insertStretch(2, 1);

}

QGroupBox* LocalPreferences3d::makeGeneralPropBox()
{
	QGroupBox* groupBox = new QGroupBox("General");
	QGridLayout* gridLayout = new QGridLayout;
	groupBox->setLayout( gridLayout );

  	return groupBox;
}
