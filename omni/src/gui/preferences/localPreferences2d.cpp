#include <QtGui>
#include "gui/guiUtils.h"
#include "gui/preferences/localPreferences2d.h"
#include "common/omDebug.h"
#include "system/omLocalPreferences.h"

LocalPreferences2d::LocalPreferences2d(QWidget * parent)
 : QWidget(parent)
{
	QVBoxLayout* overallContainer = new QVBoxLayout( this );

	overallContainer->addWidget( makeGeneralPropBox());
	overallContainer->insertStretch(2, 1);
}

QGroupBox* LocalPreferences2d::makeGeneralPropBox()
{
	QGroupBox* groupBox = new QGroupBox("General");
	QGridLayout* gridLayout = new QGridLayout;
	groupBox->setLayout( gridLayout );

  	return groupBox;
}
