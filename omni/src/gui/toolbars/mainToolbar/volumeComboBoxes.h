// FIXME!!! (purcaro)

	QComboBox *mSegmentationCombo;
	QComboBox *mChannelCombo;


void MainToolbar::filterSegmentationChanged(int)
{
	// Do What Thou Wilst . . . 
}

void MainToolbar::filterChannelChanged(int chanId)
{
	OmID filterID = 1;

	if( OmProject::IsChannelValid( chanId ) ){
		OmChannel& channel = OmProject::GetChannel( chanId );
		if( channel.IsFilterValid( filterID ) ){
			OmFilter2d & filter = OmProject::GetChannel(chanId).GetFilter(filterID);
			alphaSlider->setValue(filter.GetAlpha() * 100);
			OmEventManager::PostEvent(new OmViewEvent(OmViewEvent::REDRAW));
 		}
	}
}


void MainToolbar::setupFilterToolbar()
{

	mSegmentationCombo = new QComboBox(this);
        foreach(OmID segmentationID, OmProject::GetValidSegmentationIds()) {
		QString segString = QString("Segmentation %1").arg(segmentationID) ;
		mSegmentationCombo->insertItem(segmentationID, segString);
	
	}
	connect(mSegmentationCombo, SIGNAL(currentIndexChanged(int)), 
		this, SLOT(filterSegmentationChanged(int)), Qt::DirectConnection);
		
	mChannelCombo = new QComboBox(this);
        foreach(OmID channelID, OmProject::GetValidChannelIds()) {
		QString chanString = QString("Channel %1").arg(channelID) ;
		mChannelCombo->insertItem(channelID, chanString);
	
	}
	connect(mChannelCombo, SIGNAL(currentIndexChanged(int)), 
		this, SLOT(filterChannelChanged(int)), Qt::DirectConnection);
		

	filterToolBar->addWidget(mSegmentationCombo);
	filterToolBar->addWidget(mChannelCombo);
}

