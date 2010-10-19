#include "utility/dataWrappers.h"
#include "common/omCommon.h"
#include "common/omDebug.h"
#include "gui/elementListBox.h"
#include "gui/widgets/omSegmentListWidget.h"
#include "gui/segmentListBase.h"
#include "project/omProject.h"
#include "segment/omSegmentCache.h"
#include "system/events/omSegmentEvent.h"
#include "volume/omSegmentation.h"
#include "segment/omSegmentSelector.h"

#include <boost/make_shared.hpp>

SegmentListBase::SegmentListBase( QWidget * parent,
								  InspectorProperties * ip,
								  ElementListBox * eb )
	: QWidget(parent)
	, elementListBox(eb)
	, haveValidSDW(false)
	, currentPageNum(0)
{
	layout = new QVBoxLayout(this);

	segmentListWidget = new OmSegmentListWidget(this, ip);
	layout->addWidget(segmentListWidget);

	setupPageButtons();
}

int SegmentListBase::getNumSegmentsPerPage()
{
	return 100;
}

quint32 SegmentListBase::getTotalNumberOfSegments()
{
	assert( haveValidSDW );
	return currentSDW->getSegmentListSize(getRootSegType());
}

OmSegPtrList * SegmentListBase::getSegmentsToDisplay( const unsigned int in_offset, const bool useOffset)
{
	assert( haveValidSDW );

	unsigned int offset = in_offset - (in_offset % getNumSegmentsPerPage() );
	OmSegID startSeg = 0;
	if(!useOffset){
		startSeg = in_offset;
	}

	OmSegPtrListWithPage * segIDsAll = currentSDW->getSegmentCache()->getRootLevelSegIDs( offset, getNumSegmentsPerPage(), getRootSegType(), startSeg);
	currentPageNum = segIDsAll->mPageOffset;

	OmSegPtrList * ret = new OmSegPtrList();

	OmSegPtrList::const_iterator iter;
	for( iter = segIDsAll->list.begin(); iter != segIDsAll->list.end(); ++iter){
		ret->push_back( *iter );
	}

	delete segIDsAll;

	return ret;
}

void SegmentListBase::populateSegmentElementsListWidget(const bool doScrollToSelectedSegment,
														const OmSegID segmentJustSelectedID,
														const bool useOffset)
{
	assert( haveValidSDW );
	OmSegPtrList * segs = getSegmentsToDisplay( segmentJustSelectedID, useOffset);
	const bool shouldThisTabBeMadeActive =
		segmentListWidget->populateSegmentElementsListWidget(doScrollToSelectedSegment,
															 segmentJustSelectedID,
															 *currentSDW,
															 segs );

	delete segs;

	debug(segmentlist, "bye!\n");

	if(shouldThisTabBeMadeActive){
		makeTabActiveIfContainsJumpedToSegment();
	}

	elementListBox->addTab(getPreferredTabIndex(), this, getTabTitle() );
}

void SegmentListBase::setupPageButtons()
{
	int x = 30, y = 30;
	startButton = new QPushButton("|<<");
	startButton->setFixedSize(x, y);
	prevButton = new QPushButton("<");
	prevButton->setFixedSize(x, y);
	nextButton = new QPushButton(">");
	nextButton->setFixedSize(x, y);
	endButton = new QPushButton(">>|");
	endButton->setFixedSize(x, y);

	searchEdit = new QLineEdit();
	connect(searchEdit, SIGNAL(returnPressed()),
			this, SLOT(searchChanged()));

	connect( startButton, SIGNAL( released()  ),
			 this, SLOT( goToStartPage() ), Qt::DirectConnection);

	connect( prevButton, SIGNAL( released()  ),
			 this, SLOT( goToPrevPage() ), Qt::DirectConnection);

	connect( nextButton, SIGNAL( released()  ),
			 this, SLOT( goToNextPage() ), Qt::DirectConnection);

	connect( endButton, SIGNAL( released()  ),
			 this, SLOT( goToEndPage() ), Qt::DirectConnection);

	connect( endButton, SIGNAL( released()  ),
			 this, SLOT( goToEndPage() ), Qt::DirectConnection);

	QGroupBox * buttonBox = new QGroupBox("");
	buttonBox->setFlat(true);
	QHBoxLayout * buttons = new QHBoxLayout( buttonBox );

	buttons->addWidget(startButton);
	buttons->addWidget(prevButton);
	buttons->addWidget(nextButton);
	buttons->addWidget(endButton);
	layout->addWidget( buttonBox );

	QGroupBox * searchBox = new QGroupBox("");
	searchBox->setFlat(true);
	QHBoxLayout * searchLayout = new QHBoxLayout( searchBox );
	searchLayout->addWidget(searchEdit);
	layout->addWidget( searchBox );
}

void SegmentListBase::goToStartPage()
{
	currentPageNum = 0;
	int offset = currentPageNum * getNumSegmentsPerPage();
	populateSegmentElementsListWidget( false, offset, true);
}

void SegmentListBase::goToNextPage()
{
	++currentPageNum;
	unsigned int offset = currentPageNum * getNumSegmentsPerPage();
	if( offset > getTotalNumberOfSegments() ){
		--currentPageNum;
		offset = currentPageNum * getNumSegmentsPerPage();
	}
	populateSegmentElementsListWidget( false, offset, true);
}

void SegmentListBase::goToPrevPage()
{
	--currentPageNum;
	if( currentPageNum < 0 ){
		currentPageNum = 0;
	}
	int offset = currentPageNum * getNumSegmentsPerPage();
	populateSegmentElementsListWidget( false, offset, true);
}

void SegmentListBase::goToEndPage()
{
	currentPageNum = (getTotalNumberOfSegments() / getNumSegmentsPerPage());
	if(currentPageNum < 0) {
		currentPageNum = 0;
	}
	int offset = currentPageNum * getNumSegmentsPerPage();
	populateSegmentElementsListWidget( false, offset, true);
}

void SegmentListBase::makeSegmentationActive(SegmentationDataWrapper sdw,
											 const OmSegID segmentJustSelectedID,
											 const bool doScroll )
{
	currentSDW = boost::make_shared<SegmentationDataWrapper>(sdw);
	haveValidSDW = true;
	populateSegmentElementsListWidget(doScroll, segmentJustSelectedID);
}

void SegmentListBase::rebuildSegmentList(const OmID segmentationID,
										 const OmSegID segmentJustAddedID)
{
	makeSegmentationActive(SegmentationDataWrapper(segmentationID),
						   segmentJustAddedID,
						   true );
}

int SegmentListBase::dealWithSegmentObjectModificationEvent(OmSegmentEvent * event)
{
	bool doScroll = event->getDoScroll();

	// if we sent event, don't scroll
	if( event->getComment() == OmSegmentListWidget::eventSenderName() ){
		doScroll = false;
	}

	const OmID segmentationID = event->GetModifiedSegmentationId();

	if (OmProject::IsSegmentationValid(segmentationID)) {
		const OmSegID segmentJustSelectedID = event->GetSegmentJustSelectedID();
		makeSegmentationActive(SegmentationDataWrapper(segmentationID),
							   segmentJustSelectedID,
							   doScroll );
		return segmentationID;
	} else {
		if( haveValidSDW ){
			populateSegmentElementsListWidget();
			return currentSDW->getID();
		}
		return 0;
	}
}

void SegmentListBase::searchChanged()
{
	OmSegID segmenID = searchEdit->text().toInt();

	if(!currentSDW->getSegmentCache()->IsSegmentValid(segmenID)) {
		return;
	}

	OmSegmentSelector sel(currentSDW->getID(), NULL, "segmentlistbase");
	sel.selectJustThisSegment(segmenID, true);
	sel.sendEvent();

	makeSegmentationActive(*currentSDW, segmenID, true);
}

void SegmentListBase::userJustClickedInThisSegmentList()
{
	elementListBox->setActiveTab( this );
}
