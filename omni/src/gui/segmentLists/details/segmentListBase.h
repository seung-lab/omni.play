#ifndef SEGMENT_LIST_BASE_H
#define SEGMENT_LIST_BASE_H

#include "common/omCommon.h"
#include "utility/dataWrappers.h"

#include <QtGui>
#include <QWidget>

class GUIPageRequest;
class GUIPageOfSegments;
class OmViewGroupState;
class OmSegmentEvent;
class OmSegmentListWidget;
class SegmentInspector;
class OmViewGroupState;

class SegmentListBase : public QWidget {
    Q_OBJECT

public:
    SegmentListBase(QWidget*, OmViewGroupState*);

    void populate(){
        populateByPage(0);
    }

    void populateByPage(const int inOffset);
    void populateBySegment(const bool doScrollToSelectedSegment,
                           const SegmentDataWrapper segmentJustSelected);

    void makeSegmentationActive(const SegmentDataWrapper& sdw,
                                const bool doScroll );

    void MakeSegmentationActive(const SegmentationDataWrapper& sdw);

    void userJustClickedInThisSegmentList();
    void rebuildSegmentList(const SegmentDataWrapper& sdw);

    virtual bool shouldSelectedSegmentsBeAddedToRecentList() = 0;

public Q_SLOTS:
    void goToNextPage();
    void goToPrevPage();
    void goToEndPage();
    void goToStartPage();
    void searchChanged();

protected:
    virtual QString getTabTitle() = 0;
    virtual uint64_t Size() = 0;
    virtual int getPreferredTabIndex() = 0;
    virtual void makeTabActiveIfContainsJumpedToSegment() = 0;
    virtual om::shared_ptr<GUIPageOfSegments>
    getPageSegments(const GUIPageRequest& request) = 0;

    QVBoxLayout* layout;
    QPushButton* prevButton;
    QPushButton* nextButton;
    QPushButton* startButton;
    QPushButton* endButton;
    QLineEdit* searchEdit;

    OmSegmentListWidget* segmentListWidget;

    SegmentationDataWrapper sdw_;
    bool haveValidSDW;

    QMenu* contextMenu;
    QAction* propAct;
    int getNumSegmentsPerPage();
    void setupPageButtons();

    int currentPageNum;
    quint32 getTotalNumberOfSegments();

    OmViewGroupState * vgs_;

    void searchOne(const QString& text);
    void searchMany(const QStringList& args);
};

#endif
