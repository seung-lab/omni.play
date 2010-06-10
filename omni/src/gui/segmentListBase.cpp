#include "common/omCommon.h"
#include "common/omDebug.h"
#include "gui/elementListBox.h"
#include "gui/omSegmentListWidget.h"
#include "gui/segmentListBase.h"
#include "project/omProject.h"
#include "segment/omSegmentCache.h"
#include "system/events/omSegmentEvent.h"
#include "volume/omSegmentation.h"

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

quint32 SegmentListBase::getMaxSegmentValue()
{
	assert( haveValidSDW );
	return currentSDW.getMaxSegmentValue();
}


OmSegPtrList * SegmentListBase::getSegmentsToDisplay( const unsigned int in_offset, const bool useOffset)
{
	assert( haveValidSDW );

	unsigned int offset = in_offset - (in_offset % getNumSegmentsPerPage() );
	OmSegID startSeg = 0;
	if(!useOffset){
		startSeg = in_offset;
	}

	OmSegPtrListWithPage * segIDsAll = currentSDW.getSegmentCache()->getRootLevelSegIDs( offset, getNumSegmentsPerPage(), getRootSegType(), startSeg);
	currentPageNum = segIDsAll->mPageOffset;
	printf("currentPageNum %i\n", currentPageNum);

	OmSegPtrList * ret = new OmSegPtrList();

	OmSegPtrList::const_iterator iter;
	for( iter = segIDsAll->list.begin(); iter != segIDsAll->list.end(); ++iter){
		ret->push_back( *iter );
	}

	delete segIDsAll;

	return ret;
}

void SegmentListBase::populateSegmentElementsListWidget(const bool doScrollToSelectedSegment,
							    const OmId segmentJustSelectedID, 
							    const bool useOffset)
{
	assert( haveValidSDW );
	OmSegPtrList * segs = getSegmentsToDisplay( segmentJustSelectedID, useOffset);
	segmentListWidget->populateSegmentElementsListWidget(doScrollToSelectedSegment, 
							     segmentJustSelectedID, 
							     currentSDW,
							     segs );

	delete segs;

	elementListBox->addTab(getPreferredTabIndex(), this, getTabTitle() );
}

void SegmentListBase::setupPageButtons()
{
        prevButton = new QPushButton("<");
        nextButton = new QPushButton(">");

        connect( prevButton, SIGNAL( released()  ),
                 this, SLOT( goToPrevPage() ), Qt::DirectConnection);

        connect( nextButton, SIGNAL( released()  ),
                 this, SLOT( goToNextPage() ), Qt::DirectConnection);

        QGroupBox * buttonBox = new QGroupBox("");
        buttonBox->setFlat(true);
        QHBoxLayout * buttons = new QHBoxLayout( buttonBox );

        buttons->addWidget(prevButton);
        buttons->addWidget(nextButton);

        layout->addWidget( buttonBox );
}

void SegmentListBase::goToNextPage()
{
	currentPageNum++;
	unsigned int offset = currentPageNum * getNumSegmentsPerPage();
	if( offset > getMaxSegmentValue() ){
		currentPageNum--;
		offset = currentPageNum * getNumSegmentsPerPage();
	}
	populateSegmentElementsListWidget( false, offset, true);
}

void SegmentListBase::goToPrevPage()
{
	currentPageNum--;
	if( currentPageNum < 0 ){
		currentPageNum = 0;
	}
	int offset = currentPageNum * getNumSegmentsPerPage();
	populateSegmentElementsListWidget( false, offset, true);
}

void SegmentListBase::makeSegmentationActive(const OmId segmentationID)
{
	makeSegmentationActive( SegmentationDataWrapper(segmentationID) );
}

void SegmentListBase::makeSegmentationActive(SegmentationDataWrapper sdw)
{
	currentSDW = sdw;
	haveValidSDW = true;
	populateSegmentElementsListWidget();
}

void SegmentListBase::makeSegmentationActive(const OmId segmentationID, const OmId segmentJustSelectedID)
{
	makeSegmentationActive( SegmentationDataWrapper(segmentationID), segmentJustSelectedID);
}

void SegmentListBase::makeSegmentationActive(SegmentationDataWrapper sdw, const OmId segmentJustSelectedID)
{
	currentSDW = sdw;
	haveValidSDW = true;
	populateSegmentElementsListWidget(true, segmentJustSelectedID);
}

void SegmentListBase::rebuildSegmentList(const OmId segmentationID)
{
	makeSegmentationActive(segmentationID);
}

void SegmentListBase::rebuildSegmentList(const OmId segmentationID,
					     const OmId segmentJustAddedID)
{
	makeSegmentationActive(segmentationID, segmentJustAddedID );
}

int SegmentListBase::dealWithSegmentObjectModificationEvent(OmSegmentEvent * event)
{
	// FIXME: does this work anymore? this is no longer myinspectorwidget... (purcaro)
	// if we sent this signal, just ignore...
	if (this == event->getSender()) {
		//printf("event comment was %s\n", event->getComment().c_str() );
		return 0;
	}

	const OmId segmentationID = event->GetModifiedSegmentationId();

	if (OmProject::IsSegmentationValid(segmentationID)) {
		const OmId segmentJustSelectedID = event->GetSegmentJustSelectedID();
		makeSegmentationActive(segmentationID, segmentJustSelectedID);
		return segmentationID;
	} else {
		if( haveValidSDW ){
			populateSegmentElementsListWidget();
			return currentSDW.getID();
		}
		return 0;
	}
}
