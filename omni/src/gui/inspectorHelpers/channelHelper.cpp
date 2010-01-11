#include "channelHelper.h"

#include "../myInspectorWidget.h"

ChannelHelper::ChannelHelper( MyInspectorWidget* parent )
	: InspectorHelper<ChannelDataWrapper>( parent )
{
	
}

void ChannelHelper::addToSplitter( ChannelDataWrapper data )
{
	const OmId item_id = data.getID();

	QWidget* my_widget = mParent->splitter->widget(1);

	if((mParent->current_object != CHANNEL) || (mParent->preferencesActivated)) {
		
		QList<int> my_sizes = mParent->splitter->sizes();
		delete my_widget;	
		
		channelInspectorWidget = new ChanInspector( item_id, mParent->splitter);
		connect( channelInspectorWidget->addFilterButton, SIGNAL(clicked()), 
			    mParent, SLOT(mParent->addFilter()));
		
		if(!(mParent->first_access)) {
			mParent->splitter->setSizes(my_sizes);
		}
	}

	channelInspectorWidget->setId(item_id);
	populateChannelInspector(item_id);
	connect( channelInspectorWidget->nameEdit, SIGNAL(editingFinished()), 
		    mParent, SLOT(mParent->nameEditChanged()), Qt::DirectConnection);
	mParent->current_object = CHANNEL;
}


void ChannelHelper::populateChannelInspector(OmId c_id)
{
	
	OmChannel &current_channel = OmVolume::GetChannel(c_id);
	
	const string &my_name = current_channel.GetName();
	channelInspectorWidget->nameEdit->setText(QString::fromStdString(my_name));
	channelInspectorWidget->nameEdit->setMinimumWidth(200);
	
	const string &my_directory = current_channel.GetSourceDirectoryPath();
	channelInspectorWidget->directoryEdit->setText(QString::fromStdString(my_directory));
	channelInspectorWidget->directoryEdit->setMinimumWidth(200);
	
	const string &my_pattern = current_channel.GetSourceFilenameRegex();
	channelInspectorWidget->patternEdit->setText(QString::fromStdString(my_pattern));
	channelInspectorWidget->patternEdit->setMinimumWidth(200);
	
	channelInspectorWidget->listWidget->clear();
	list<string>::const_iterator match_it;
	const list<string> &matches = OmVolume::GetChannel(c_id).GetSourceFilenameRegexMatches();
	
	for(match_it = matches.begin(); match_it != matches.end(); ++match_it) {
		channelInspectorWidget->listWidget->addItem(QString::fromStdString(*match_it));
	}
	
	const string &my_notes = current_channel.GetNote();
	channelInspectorWidget->notesEdit->setPlainText(QString::fromStdString(my_notes));
}
