#include "actions/omSelectSegmentParams.hpp"
#include "common/omCommon.h"
#include "common/omDebug.h"
#include "gui/segmentLists/details/segmentListBase.h"
#include "gui/segmentLists/elementListBox.hpp"
#include "gui/widgets/omSegmentListWidget.h"
#include "project/omProject.h"
#include "segment/omSegmentCache.h"
#include "segment/omSegmentLists.hpp"
#include "segment/omSegmentSearched.hpp"
#include "segment/omSegmentSelector.h"
#include "segment/omSegmentUtils.hpp"
#include "events/omSegmentEvent.h"
#include "utility/dataWrappers.h"
#include "volume/omSegmentation.h"

SegmentListBase::SegmentListBase( QWidget * parent,
                                  OmViewGroupState* vgs)
    : QWidget(parent)
    , haveValidSDW(false)
    , currentPageNum(0)
    , vgs_(vgs)
{
    layout = new QVBoxLayout(this);

    segmentListWidget = new OmSegmentListWidget(this, vgs);
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
    return Size();
}

boost::shared_ptr<OmSegIDsListWithPage>
SegmentListBase::getSegmentsToDisplay( const unsigned int in_offset,
                                       const bool useOffset)
{
    assert( haveValidSDW );

    const unsigned int offset = in_offset - (in_offset % getNumSegmentsPerPage() );
    OmSegID startSeg = 0;
    if(!useOffset){
        startSeg = in_offset;
    }

    boost::shared_ptr<OmSegIDsListWithPage> ids =
        getPageSegments(offset, getNumSegmentsPerPage(), startSeg);

    currentPageNum = ids->Offset();

    return ids;
}

void SegmentListBase::populate(const bool doScrollToSelectedSegment,
                               const OmSegID segmentJustSelectedID,
                               const bool useOffset)
{
    assert( haveValidSDW );

    boost::shared_ptr<OmSegIDsListWithPage> segIDs =
        getSegmentsToDisplay( segmentJustSelectedID, useOffset);

    const bool shouldThisTabBeMadeActive =
        segmentListWidget->populate(doScrollToSelectedSegment,
                                    segmentJustSelectedID,
                                    sdw_,
                                    segIDs);

    debug(segmentlist, "bye!\n");

    if(shouldThisTabBeMadeActive){
        makeTabActiveIfContainsJumpedToSegment();
    }

    ElementListBox::AddTab(getPreferredTabIndex(), this, getTabTitle());
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
    populate( false, offset, true);
}

void SegmentListBase::goToNextPage()
{
    ++currentPageNum;
    unsigned int offset = currentPageNum * getNumSegmentsPerPage();
    if( offset > getTotalNumberOfSegments() ){
        --currentPageNum;
        offset = currentPageNum * getNumSegmentsPerPage();
    }
    populate( false, offset, true);
}

void SegmentListBase::goToPrevPage()
{
    --currentPageNum;
    if( currentPageNum < 0 ){
        currentPageNum = 0;
    }
    int offset = currentPageNum * getNumSegmentsPerPage();
    populate( false, offset, true);
}

void SegmentListBase::goToEndPage()
{
    currentPageNum = (getTotalNumberOfSegments() / getNumSegmentsPerPage());
    if(currentPageNum < 0) {
        currentPageNum = 0;
    }
    int offset = currentPageNum * getNumSegmentsPerPage();
    populate( false, offset, true);
}

void SegmentListBase::makeSegmentationActive(const SegmentDataWrapper& sdw,
                                             const bool doScroll )
{
    sdw_ = sdw.MakeSegmentationDataWrapper();
    haveValidSDW = true;
    populate(doScroll, sdw.GetSegmentID());
}

void SegmentListBase::rebuildSegmentList(const SegmentDataWrapper& sdw)
{
    makeSegmentationActive(sdw, true);
}

SegmentationDataWrapper
SegmentListBase::dealWithSegmentObjectModificationEvent(OmSegmentEvent* event)
{
    bool doScroll = event->Params().shouldScroll;

    // if we sent event, don't scroll
    if( event->Params().comment == OmSegmentListWidget::eventSenderName() ){
        doScroll = false;
    }

    const SegmentDataWrapper& sdw = event->Params().sdw;

    SegmentationDataWrapper newsdw;

    if(sdw.IsSegmentationValid()){
        makeSegmentationActive(sdw, doScroll );
        newsdw = sdw.MakeSegmentationDataWrapper();

    } else {
        if( haveValidSDW ){
            populate();
            newsdw = sdw_;
        } else {
            newsdw = SegmentationDataWrapper();
        }
    }

    if(newsdw.IsSegmentationValid()){
        if(event->Params().autoCenter){
            OmSegmentUtils::CenterSegment(vgs_, newsdw);
        }
    }

    return newsdw;
}

void SegmentListBase::searchChanged()
{
    const OmSegID segID = searchEdit->text().toInt();
    SegmentDataWrapper sdw(sdw_, segID);

    if(!sdw.IsSegmentValid()){
        return;
    }

    OmSegmentSearched::Set(sdw);

    OmSegmentSelector sel(sdw_, NULL, "segmentlistbase");
    sel.selectJustThisSegment(segID, true);
    sel.sendEvent();

    makeSegmentationActive(sdw, true);

    OmSegmentUtils::CenterSegment(vgs_, sdw);
}

void SegmentListBase::userJustClickedInThisSegmentList()
{
    ElementListBox::SetActiveTab(this);
}
