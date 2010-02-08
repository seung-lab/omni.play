#include "segmentationHelper.h"

#include "../myInspectorWidget.h"

SegmentationHelper::SegmentationHelper(MyInspectorWidget * parent)
 : InspectorHelper < SegmentationDataWrapper > (parent)
{
}

void SegmentationHelper::addToSplitter(SegmentationDataWrapper data)
{
	const OmId item_id = data.getID();

	QWidget *my_widget = mParent->splitter->widget(1);

	if ((mParent->current_object != SEGMENTATION) || (mParent->preferencesActivated)) {

		QList < int >my_sizes = mParent->splitter->sizes();
		delete my_widget;

		segInspectorWidget = new SegInspector(item_id, mParent->splitter);

		connect(segInspectorWidget, SIGNAL(segmentationBuilt(OmId)),
			mParent, SLOT(mParent->rebuildSegmentList(OmId)));

		connect(segInspectorWidget->addSegmentButton, SIGNAL(clicked()), mParent, SLOT(mParent->addSegment()));

		if (!(mParent->first_access))
			mParent->splitter->setSizes(my_sizes);
	}

	segInspectorWidget->setId(item_id);
	populateSegmentationInspector(item_id);

	connect(segInspectorWidget->nameEdit, SIGNAL(editingFinished()),
		mParent, SLOT(mParent->nameEditChanged()), Qt::DirectConnection);
	mParent->current_object = SEGMENTATION;
}

void SegmentationHelper::populateSegmentationInspector(OmId s_id)
{
	OmSegmentation & current_segmentation = OmVolume::GetSegmentation(s_id);

	const string & my_name = current_segmentation.GetName();
	segInspectorWidget->nameEdit->setText(QString::fromStdString(my_name));
	segInspectorWidget->nameEdit->setMinimumWidth(200);

	const string & my_directory = current_segmentation.GetSourceDirectoryPath();
	segInspectorWidget->directoryEdit->setText(QString::fromStdString(my_directory));
	segInspectorWidget->directoryEdit->setMinimumWidth(200);

	const string & my_pattern = current_segmentation.GetSourceFilenameRegex();
	segInspectorWidget->patternEdit->setText(QString::fromStdString(my_pattern));
	segInspectorWidget->patternEdit->setMinimumWidth(200);

	const string & my_notes = current_segmentation.GetNote();
	segInspectorWidget->notesEdit->setPlainText(QString::fromStdString(my_notes));

	segInspectorWidget->listWidget->clear();

	list < string >::const_iterator match_it;
	const list < string > &matches = OmVolume::GetSegmentation(s_id).GetSourceFilenameRegexMatches();
	for (match_it = matches.begin(); match_it != matches.end(); ++match_it) {
		segInspectorWidget->listWidget->addItem(QString::fromStdString(*match_it));
	}
}
