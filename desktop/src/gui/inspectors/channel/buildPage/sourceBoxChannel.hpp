#pragma once
#include "precomp.h"

#include "system/omConnect.hpp"
#include "utility/sortHelpers.h"
#include "utility/channelDataWrapper.hpp"

namespace om {
namespace channelInspector {

class SourceBox : public QGroupBox {
  Q_OBJECT;

 private:
  const ChannelDataWrapper& cdw_;

  QLineEdit* volNameEdit_;
  QLabel* directoryLabel_;
  QLineEdit* directoryEdit_;
  QLineEdit* regExEdit_;
  QListWidget* fileList_;

 public:
  SourceBox(QWidget* parent, const QString& title,
            const ChannelDataWrapper& cdw)
      : QGroupBox(title, parent), cdw_(cdw) {
    addWidgets();
    populateFileListWidget();
  }

  QFileInfoList GetFilesToBuild() {
    QFileInfoList files = getSelectedFileNames();
    if (files.empty()) {
      files = getFilteredDir().entryInfoList();
    }

    return SortHelpers::SortNaturally(files);
  }

 private
Q_SLOTS:
  void browse() {
    QString dir = QFileDialog::getExistingDirectory(
        this, tr("Choose Directory"), "", QFileDialog::ShowDirsOnly);
    if (dir != "") {
      directoryEdit_->setText(dir);
      updateFileList();
    }
  }

  void regexChanged() { updateFileList(); }

  void changeChannelName() {
    cdw_.GetChannel().SetCustomName(volNameEdit_->text());
  }

 private:
  QFileInfoList getSelectedFileNames() {
    QList<QListWidgetItem*> selectedItems = fileList_->selectedItems();

    QFileInfoList ret;

    QDir dir = getDirPath();

    FOR_EACH(iter, selectedItems) {
      QListWidgetItem* item = *iter;
      ret.push_back(QFileInfo(dir, item->text()));
    }

    return ret;
  }

  QDir getDirPath() { return QDir(directoryEdit_->text()); }

  QDir getFilteredDir() {
    const QString regex = regExEdit_->text();
    QDir dir = getDirPath();

    dir.setSorting(QDir::Name);
    dir.setFilter(QDir::Files);

    QStringList filters;
    filters << regex;

    dir.setNameFilters(filters);

    return dir;
  }

  QStringList getFileList() {
    QStringList files = getFilteredDir().entryList();
    return SortHelpers::SortNaturally(files);
  }

  void updateFileList() {
    fileList_->clear();

    const QStringList files = getFileList();
    FOR_EACH(iter, files) { fileList_->addItem(*iter); }

    fileList_->update();
  }

  void addVolNameWidget(QGridLayout* grid, const int row) {
    QLabel* nameLabel = new QLabel(this);
    nameLabel->setObjectName(QString::fromUtf8("nameLabel"));
    nameLabel->setText("Name:");
    grid->addWidget(nameLabel, row, 0);

    volNameEdit_ = new QLineEdit(this);
    volNameEdit_->setObjectName(QString::fromUtf8("volNameEdit_"));
    volNameEdit_->setMinimumWidth(200);

    volNameEdit_->setText(cdw_.GetName());

    om::connect(volNameEdit_, SIGNAL(editingFinished()), this,
                SLOT(changeChannelName()));
    grid->addWidget(volNameEdit_, row, 1);
  }

  void addDirWidget(QGridLayout* grid, const int row) {
    directoryLabel_ = new QLabel(this);
    directoryLabel_->setObjectName(QString::fromUtf8("directoryLabel_"));
    directoryLabel_->setText("Directory:");
    grid->addWidget(directoryLabel_, row, 0);

    directoryEdit_ = new QLineEdit(this);
    directoryEdit_->setObjectName(QString::fromUtf8("directoryEdit_"));
    directoryEdit_->setMinimumWidth(200);
    directoryEdit_->setReadOnly(true);
    grid->addWidget(directoryEdit_, row, 1);
  }

  void addBrowseButton(QGridLayout* grid, const int row) {
    QPushButton* browseButton = new QPushButton(this);
    browseButton->setObjectName(QString::fromUtf8("browseButton"));
    browseButton->setText("Browse");

    om::connect(browseButton, SIGNAL(clicked()), this, SLOT(browse()));
    grid->addWidget(browseButton, row, 1);
  }

  void addRegExBox(QGridLayout* grid, const int row) {
    QLabel* patternLabel = new QLabel(this);
    patternLabel->setObjectName(QString::fromUtf8("patternLabel"));
    patternLabel->setText("Pattern:");
    patternLabel->setToolTip("(i.e. data.%d.tif)");
    grid->addWidget(patternLabel, row, 0);

    regExEdit_ = new QLineEdit(this);
    regExEdit_->setObjectName(QString::fromUtf8("regExEdit_"));
    regExEdit_->setText("*");
    regExEdit_->setMinimumWidth(200);

    om::connect(regExEdit_, SIGNAL(textChanged(const QString&)), this,
                SLOT(regexChanged()));
    grid->addWidget(regExEdit_, row, 1);
  }

  void addFileListWidget(QGridLayout* grid, const int row) {
    fileList_ = new QListWidget(this);
    fileList_->setObjectName(QString::fromUtf8("fileList_"));
    QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(fileList_->sizePolicy().hasHeightForWidth());
    fileList_->setSizePolicy(sizePolicy1);
    fileList_->setMaximumSize(QSize(16777215, 85));

    grid->addWidget(fileList_, row, 1);
  }

  void addWidgets() {
    QGridLayout* grid = new QGridLayout(this);

    addVolNameWidget(grid, 0);
    addDirWidget(grid, 1);
    addBrowseButton(grid, 2);
    addRegExBox(grid, 3);
    addFileListWidget(grid, 4);
  }

  void populateFileListWidget() {
    // TODO: use path from where import files were orginally...

    const QString folder = QFileInfo(OmProject::OmniFile()).absolutePath();
    directoryEdit_->setText(folder);

    updateFileList();
  }
};

}  // namespace channelInspector
}  // namespace om
