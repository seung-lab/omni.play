#include "system/omConnect.hpp"
#include "common/omDebug.h"
#include "events/omEvents.h"
#include "segmentInspector.h"
#include "segment/omSegmentUtils.hpp"
#include "system/cache/omCacheManager.h"
#include "utility/dataWrappers.h"
#include "utility/omStringHelpers.h"

SegmentInspector::SegmentInspector(const SegmentDataWrapper& sdw,
                                   QWidget* parent)
    : QWidget(parent), sdw_(sdw) {
  QVBoxLayout* overallContainer = new QVBoxLayout(this);
  overallContainer->addWidget(makeSourcesBox());
  overallContainer->addWidget(makeNotesBox());

  set_initial_values();

  om::connect(colorButton, SIGNAL(clicked()), this, SLOT(setSegObjColor()));

  om::connect(nameEdit, SIGNAL(editingFinished()), this,
              SLOT(nameEditChanged()));
}

void SegmentInspector::set_initial_values() {
  segmentIDEdit->setReadOnly(true);

  nameEdit->setText(sdw_.getName());
  nameEdit->setMinimumWidth(200);

  segmentIDEdit->setText(sdw_.getIDstr());
  segmentIDEdit->setMinimumWidth(200);

  notesEdit->setPlainText(sdw_.getNote());

  const QPixmap pixm = om::utils::color::OmColorAsQPixmap(sdw_.GetColorInt());
  colorButton->setIcon(QIcon(pixm));

  sizeNoChildren->setText(OmStringHelpers::HumanizeNumQT(sdw_.getSize()));
  sizeWithChildren->setText(
      OmStringHelpers::HumanizeNumQT(sdw_.GetSizeWithChildren()));

  const std::string listType =
      OmSegmentUtils::ListTypeAsStr(sdw_.GetSegment()->GetListType());
  listType_->setText(QString::fromStdString(listType));

  origDataValueList->setText(sdw_.getIDstr());
  chunkList->setText("disabled");
}

void SegmentInspector::nameEditChanged() {
  sdw_.setName(nameEdit->text());
  OmEvents::SegmentModified();
}

void SegmentInspector::setSegObjColor() {
  const QColor currentColor = OmSegmentUtils::SegColorAsQColor(sdw_);

  QColor color = QColorDialog::getColor(currentColor, this);

  if (!color.isValid()) {
    return;
  }

  color = OmSegmentUtils::SetSegColor(sdw_, color);

  std::cout << "set color to " << color << "\n";

  const QPixmap pixm = om::utils::color::OmColorAsQPixmap(sdw_.GetColorInt());
  colorButton->setIcon(QIcon(pixm));

  colorButton->update();

  OmSegmentSelected::Set(sdw_);
  OmCacheManager::TouchFreshness();
  OmEvents::Redraw2d();
  OmEvents::Redraw3d();
}

QGroupBox* SegmentInspector::makeSourcesBox() {
  QGroupBox* sourceBox = new QGroupBox("Source Properties");
  QGridLayout* grid = new QGridLayout(sourceBox);

  QLabel* nameLabel = new QLabel(sourceBox);
  nameLabel->setObjectName(QLatin1String("nameLabel"));
  nameLabel->setText("Name:");
  grid->addWidget(nameLabel, 0, 0);

  nameEdit = new QLineEdit(sourceBox);
  nameEdit->setObjectName(QLatin1String("nameEdit"));
  nameEdit->setText(QString());
  grid->addWidget(nameEdit, 0, 1);

  QLabel* segmentIDLabel = new QLabel(sourceBox);
  segmentIDLabel->setObjectName(QLatin1String("segmentIDLabel"));
  segmentIDLabel->setText("Segment ID:");
  grid->addWidget(segmentIDLabel, 1, 0);

  segmentIDEdit = new QLineEdit(sourceBox);
  segmentIDEdit->setObjectName(QLatin1String("segmentIDEdit"));
  segmentIDEdit->setText(QString());
  grid->addWidget(segmentIDEdit, 1, 1);

  QLabel* tagsLabel = new QLabel(sourceBox);
  tagsLabel->setObjectName(QLatin1String("tagsLabel"));
  tagsLabel->setText("Tags:");
  grid->addWidget(tagsLabel, 2, 0);

  tagsEdit = new QLineEdit(sourceBox);
  tagsEdit->setObjectName(QLatin1String(""));
  tagsEdit->setText(QString());
  grid->addWidget(tagsEdit, 2, 1);

  QLabel* colorLabel = new QLabel(sourceBox);
  colorLabel->setObjectName(QLatin1String("colorLabel"));
  colorLabel->setText("Color:");
  grid->addWidget(colorLabel, 3, 0);

  colorButton = new QPushButton(sourceBox);
  colorButton->setObjectName(QLatin1String("colorButton"));
  grid->addWidget(colorButton, 3, 1);

  QLabel* origDataValueLabel = new QLabel(sourceBox);
  origDataValueLabel->setObjectName(QLatin1String("origDataValueLabel"));
  origDataValueLabel->setText("Original Data value:");
  grid->addWidget(origDataValueLabel, 4, 0);

  origDataValueList = new QLabel(sourceBox);
  origDataValueList->setObjectName(QLatin1String("origDataValueList"));
  grid->addWidget(origDataValueList, 4, 1);

  QLabel* sizeLabelNoChildren = new QLabel(sourceBox);
  sizeLabelNoChildren->setObjectName(QLatin1String("sizeLabelNoChildren"));
  sizeLabelNoChildren->setText("Size (voxels--no children):");
  grid->addWidget(sizeLabelNoChildren, 5, 0);

  sizeNoChildren = new QLabel(sourceBox);
  sizeNoChildren->setObjectName(QLatin1String("sizeNoChildren"));
  grid->addWidget(sizeNoChildren, 5, 1);

  QLabel* sizeLabelWithChildren = new QLabel(sourceBox);
  sizeLabelWithChildren->setObjectName(QLatin1String("sizeLabelWithChildren"));
  sizeLabelWithChildren->setText("Size (voxels--with children):");
  grid->addWidget(sizeLabelWithChildren, 6, 0);

  sizeWithChildren = new QLabel(sourceBox);
  sizeWithChildren->setObjectName(QLatin1String("sizeWithChildren"));
  grid->addWidget(sizeWithChildren, 6, 1);

  QLabel* listTypeLabel = new QLabel(sourceBox);
  listTypeLabel->setObjectName(QLatin1String("listTypeLabel"));
  listTypeLabel->setText("List type (working, valid, uncertain)");
  grid->addWidget(listTypeLabel, 7, 0);

  listType_ = new QLabel(sourceBox);
  listType_->setObjectName(QLatin1String("list type"));
  grid->addWidget(listType_, 7, 1);

  QLabel* chunkLabel = new QLabel(sourceBox);
  chunkLabel->setObjectName(QLatin1String("chunkLabel"));
  chunkLabel->setText("Chunks");
  grid->addWidget(chunkLabel, 8, 0);

  chunkList = new QLabel(sourceBox);
  chunkList->setObjectName(QLatin1String("chunkList"));
  grid->addWidget(chunkList, 8, 1);

  return sourceBox;
}

QGroupBox* SegmentInspector::makeNotesBox() {
  QGroupBox* notesBox = new QGroupBox("Notes");

  QGridLayout* gridNotes = new QGridLayout(notesBox);

  notesEdit = new QPlainTextEdit(notesBox);
  notesEdit->setObjectName(QLatin1String("notesEdit"));
  gridNotes->addWidget(notesEdit, 0, 1);

  return notesBox;
}
