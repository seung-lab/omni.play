#include "utility/dataWrappers.h"
#include "filObjectInspector.h"
#include "common/omDebug.h"
#include "system/omEventManager.h"
#include "system/events/omViewEvent.h"

FilObjectInspector::FilObjectInspector(QWidget * parent, const FilterDataWrapper & fdw )
 : QWidget(parent)
{
	mFDW = boost::make_shared<FilterDataWrapper>(fdw);
	mChannelID = mFDW->getChannelID();
	mFilterID = mFDW->getID();

	QVBoxLayout* overallContainer = new QVBoxLayout( this );

	QGroupBox* groupBox = new QGroupBox("Filter");
	overallContainer->addWidget( groupBox );

	QGridLayout *grid = new QGridLayout( groupBox );

	grid->addWidget( makeFilterOptionsBox(), 0, 0 );
	grid->addWidget( makeNotesBox(), 1, 0 );
	grid->setRowStretch( 2, 1 );

	set_initial_values();

	connect(chanEdit, SIGNAL(editingFinished()),
		this, SLOT(sourceEditChangedChan()), Qt::DirectConnection);
	connect(segEdit, SIGNAL(editingFinished()),
		this, SLOT(sourceEditChangedSeg()), Qt::DirectConnection);
	connect(alphaSlider, SIGNAL(valueChanged(int)),
		this, SLOT(setFilAlpha(int)), Qt::DirectConnection);
	connect(alphaSlider, SIGNAL(sliderReleased()),
		this, SLOT(saveFilterAlphaValue()), Qt::DirectConnection);
}

void FilObjectInspector::saveFilterAlphaValue()
{
	//	OmProject::Save();
}

void FilObjectInspector::setFilAlpha(int alpha)
{
	if( OmProject::IsChannelValid( mChannelID ) ){
		OmChannel& channel = OmProject::GetChannel( mChannelID);
		if( channel.IsFilterValid( mFilterID ) ){
			////debug(filter, "setting alpha\n");
			channel.GetFilter( mFilterID).SetAlpha((double)alpha / 100.00);
			OmEventManager::PostEvent(new OmViewEvent(OmViewEvent::REDRAW));
 		}
	}
}

void FilObjectInspector::set_initial_values()
{
	OmFilter2d & filter = OmProject::GetChannel(mChannelID).GetFilter(mFilterID);

	alphaSlider->setValue(filter.GetAlpha() * 100);
	chanEdit->setText(QString::number(filter.GetChannel()));
	segEdit->setText(QString::number(filter.GetSegmentation()));
}

QGroupBox* FilObjectInspector::makeFilterOptionsBox()
{
	QGroupBox* groupBox = new QGroupBox("Filter Options");
	QGridLayout* gridLayout = new QGridLayout;
	groupBox->setLayout( gridLayout );

	QLabel* chanLabel = new QLabel(groupBox);
	chanLabel->setObjectName(QString::fromUtf8("chanLabel"));
	chanLabel->setText("Source Channel ID:");
	gridLayout->addWidget(chanLabel, 0, 0, 1, 1);

	chanEdit = new QLineEdit(groupBox);
	chanEdit->setObjectName(QString::fromUtf8("chanEdit"));
	gridLayout->addWidget(chanEdit, 0, 1, 1, 1);

	QLabel* segLabel = new QLabel(groupBox);
	segLabel->setObjectName(QString::fromUtf8("segLabel"));
	segLabel->setText("Source Segmentation ID:");
	gridLayout->addWidget(segLabel, 1, 0, 1, 1);

	segEdit = new QLineEdit(groupBox);
	segEdit->setObjectName(QString::fromUtf8("segEdit"));
	gridLayout->addWidget(segEdit, 1, 1, 1, 1);

	QLabel* tagsLabel = new QLabel(groupBox);
	tagsLabel->setObjectName(QString::fromUtf8("tagsLabel"));
	tagsLabel->setText("Tags:");
	gridLayout->addWidget(tagsLabel, 2, 0, 1, 1);

	tagsEdit = new QLineEdit(groupBox);
	tagsEdit->setObjectName(QString::fromUtf8("tagsEdit"));
	gridLayout->addWidget(tagsEdit, 2, 1, 1, 1);

	QLabel* alphaLabel = new QLabel(groupBox);
	alphaLabel->setObjectName(QString::fromUtf8("alphaLabel"));
	alphaLabel->setText("Alpha:");
	gridLayout->addWidget(alphaLabel, 3, 0, 1, 1);

	alphaSlider = new QSlider(Qt::Horizontal, groupBox);
	alphaSlider->setObjectName(QString::fromUtf8("alphaSlider"));
	gridLayout->addWidget(alphaSlider, 3, 1, 1, 1);

	return groupBox;
}

QGroupBox* FilObjectInspector::makeNotesBox()
{
	QGroupBox* groupBox = new QGroupBox("Notes");
	QVBoxLayout* vbox = new QVBoxLayout();
	groupBox->setLayout( vbox );

        notesEdit = new QPlainTextEdit(groupBox);
        notesEdit->setObjectName(QString("notesEdit"));
	vbox->addWidget(notesEdit);

	return groupBox;
}

int FilObjectInspector::getChannelIDtoFilter()
{
	return chanEdit->text().toInt();
}

int FilObjectInspector::getSegmentationIDtoFilter()
{
	return segEdit->text().toInt();
}

void FilObjectInspector::sourceEditChangedChan()
{
	OmProject::GetChannel( mChannelID ).GetFilter( mFilterID ).SetChannel(getChannelIDtoFilter());
	//	OmProject::Save();
}

void FilObjectInspector::sourceEditChangedSeg()
{
	OmProject::GetChannel( mChannelID ).GetFilter( mFilterID ).SetSegmentation(getSegmentationIDtoFilter());
	//	OmProject::Save();
}
