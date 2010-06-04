#include "common/omCommon.h"
#include "common/omDebug.h"
#include "gui/elementListBox.h"
#include "gui/omSegmentListWidget.h"
#include "gui/segmentListAbstract.h"
#include "project/omProject.h"
#include "segment/omSegmentCache.h"
#include "system/events/omSegmentEvent.h"
#include "volume/omSegmentation.h"

SegmentListAbstract::SegmentListAbstract( QWidget * parent, 
					  InspectorProperties * ip,
					  ElementListBox * eb ) 
	: QWidget(parent)
	, segmentListWidget(NULL)
	, elementListBox(eb)
	, haveValidSDW(false)
	, currentPageNum(0)
{
        layout = new QVBoxLayout(this);

	segmentListWidget = new OmSegmentListWidget(this, ip);
	layout->addWidget(segmentListWidget);
	
	setupPageButtons();
}

int SegmentListAbstract::getNumSegmentsPerPage()
{
	return 100;
}

quint32 SegmentListAbstract::getMaxSegmentValue()
{
	assert( haveValidSDW );
	return currentSDW.getMaxSegmentValue();
}

OmSegPtrList * SegmentListAbstract::getSegmentsToDisplay( const OmId firstSegmentID )
{
	assert( haveValidSDW );

	int offset = firstSegmentID - (firstSegmentID % getNumSegmentsPerPage() );
	return doGetSegmentsToDisplay( offset );
}

OmSegPtrList * SegmentListAbstract::doGetSegmentsToDisplay( const unsigned int in_offset )
{
	assert( haveValidSDW );

	unsigned int offset = 0;
	if( getMaxSegmentValue() > in_offset ){
		offset = in_offset;
	}

	OmSegPtrList * segIDsAll = currentSDW.getSegmentCache()->getRootLevelSegIDs( offset, getNumSegmentsPerPage() );

	OmSegPtrList * ret = new OmSegPtrList();

	OmSegPtrList::const_iterator iter;
	for( iter = segIDsAll->begin(); iter != segIDsAll->end(); ++iter){
		if( !shouldSegmentBeAdded( *iter ) ){
			continue;
		}
		ret->push_back( *iter );
	}

	delete segIDsAll;

	return ret;
}

void SegmentListAbstract::populateSegmentElementsListWidget(const bool doScrollToSelectedSegment,
							    const OmId segmentJustSelectedID)
{
	assert( haveValidSDW );
	OmSegPtrList * segs = getSegmentsToDisplay( segmentJustSelectedID );
	segmentListWidget->populateSegmentElementsListWidget(doScrollToSelectedSegment, 
							     segmentJustSelectedID, 
							     currentSDW,
							     segs );

	delete segs;

	elementListBox->addTab(0, getGroupBoxTitle(),
			       this, getTabTitle() );
}

void SegmentListAbstract::setupPageButtons()
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

void SegmentListAbstract::goToNextPage()
{
	currentPageNum++;
	unsigned int offset = currentPageNum * getNumSegmentsPerPage();
	if( offset > getMaxSegmentValue() ){
		currentPageNum--;
		offset = currentPageNum * getNumSegmentsPerPage();
	}
	populateSegmentElementsListWidget( false, offset );
}

void SegmentListAbstract::goToPrevPage()
{
	currentPageNum--;
	if( currentPageNum < 0 ){
		currentPageNum = 0;
	}
	int offset = currentPageNum * getNumSegmentsPerPage();
	populateSegmentElementsListWidget( false, offset );
}

void SegmentListAbstract::makeSegmentationActive(const OmId segmentationID)
{
	makeSegmentationActive( SegmentationDataWrapper(segmentationID) );
}

void SegmentListAbstract::makeSegmentationActive(SegmentationDataWrapper sdw)
{
	currentSDW = sdw;
	haveValidSDW = true;
	populateSegmentElementsListWidget();
}

void SegmentListAbstract::makeSegmentationActive(const OmId segmentationID, const OmId segmentJustSelectedID)
{
	makeSegmentationActive( SegmentationDataWrapper(segmentationID), segmentJustSelectedID);
}

void SegmentListAbstract::makeSegmentationActive(SegmentationDataWrapper sdw, const OmId segmentJustSelectedID)
{
	currentSDW = sdw;
	haveValidSDW = true;
	populateSegmentElementsListWidget(true, segmentJustSelectedID);
}

void SegmentListAbstract::rebuildSegmentList(const OmId segmentationID)
{
	makeSegmentationActive(segmentationID);
}

void SegmentListAbstract::rebuildSegmentList(const OmId segmentationID,
					     const OmId segmentJustAddedID)
{
	makeSegmentationActive(segmentationID, segmentJustAddedID );
}

void SegmentListAbstract::dealWithSegmentObjectModificationEvent(OmSegmentEvent * event)
{
	// quick hack; assumes userData is pointer to sender (and we're the only
	//  ones to set the sender...)
	if (this == event->getSender()) {
		debug("guievent", "in SegmentList:%s: i sent it! (%s)\n", __FUNCTION__, event->getComment().c_str());
		return;
	} else {
		debug("guievent", "in SegmentList:%s: i did NOT send it! (%s)\n", __FUNCTION__, event->getComment().c_str());
	}

	const OmId segmentationID = event->GetModifiedSegmentationId();
	if (!OmProject::IsSegmentationValid(segmentationID)) {
		if( haveValidSDW ){
			populateSegmentElementsListWidget();
		}
		return;
	}

	const OmId segmentJustSelectedID = event->GetSegmentJustSelectedID();

	makeSegmentationActive(segmentationID, segmentJustSelectedID);
}


QString SegmentListAbstract::getGroupBoxTitle()
{
	return QString("Segmentation %1: Segments").arg(currentSDW.getID());
}
