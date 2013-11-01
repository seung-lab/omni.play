#pragma once

#include <memory>
#include <QtGui>
#include <QWidget>

#include "utility/segmentationDataWrapper.hpp"

class SegmentDataWrapper;
class GUIPageRequest;
class GUIPageOfSegments;
class OmViewGroupState;
class OmSegmentEvent;
class OmSegmentListWidget;
class SegmentInspector;
class OmViewGroupState;

class SegmentListBase : public QWidget {
  Q_OBJECT;
  ;
 public:
  SegmentListBase(QWidget*, OmViewGroupState*);

  void populate() { populateByPage(); }

  void populateByPage();
  void populateBySegment(const bool doScrollToSelectedSegment,
                         const SegmentDataWrapper segmentJustSelected);

  void makeSegmentationActive(const SegmentDataWrapper& sdw,
                              const bool doScroll);

  void MakeSegmentationActive(const SegmentationDataWrapper& sdw);

  void userJustClickedInThisSegmentList();
  void rebuildSegmentList(const SegmentDataWrapper& sdw);

  void RefreshPage(const SegmentationDataWrapper& sdw);

  virtual bool shouldSelectedSegmentsBeAddedToRecentList() = 0;

 public
Q_SLOTS:
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
  virtual std::shared_ptr<GUIPageOfSegments> getPageSegments(
      const GUIPageRequest& request) = 0;

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

  int currentPageNum_;
  quint32 getTotalNumberOfSegments();

  OmViewGroupState* vgs_;

  void searchOne(const QString& text);
  void searchMany(const QStringList& args);
};
