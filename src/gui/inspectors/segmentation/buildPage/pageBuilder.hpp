#pragma once

#include "gui/widgets/progressBarDialog.hpp"
#include "gui/inspectors/segmentation/buildPage/segVolBuilder.hpp"
#include "gui/inspectors/segmentation/buildPage/sourceBox.hpp"

namespace om {
namespace segmentationInspector {

class PageBuilder : public QWidget {
Q_OBJECT

private:
    const SegmentationDataWrapper sdw_;

    SourceBox *const sourceBox_;
    QComboBox* buildComboBox_;

public:
    PageBuilder(QWidget* parent, const SegmentationDataWrapper& sdw)
        : QWidget(parent)
        , sdw_(sdw)
        , sourceBox_(new SourceBox(this, "Source Files", sdw))
    {
        QVBoxLayout* overallContainer = new QVBoxLayout(this);
        overallContainer->addWidget(sourceBox_);
        overallContainer->addLayout(makeActionsBox());
        overallContainer->addStretch(1);
    }

private Q_SLOTS:
    void build()
    {
        om::gui::progressBarDialog* dialog = new om::gui::progressBarDialog(NULL);

        dialog->push_back(
            zi::run_fn(
                zi::bind(PageBuilder::doBuild,
                         sdw_,
                         buildComboBox_->currentText(),
                         sourceBox_->GetFilesToBuild(),
                         dialog)));
    }

private:
    static void doBuild(const SegmentationDataWrapper sdw,
                        const QString whatOrHowToBuild,
                        const QFileInfoList fileNamesAndPaths,
                        om::gui::progressBarDialog* dialog)
    {
        segVolBuilder builder(sdw, whatOrHowToBuild, fileNamesAndPaths);
        builder.Build(dialog->Progress());
    }

    QGridLayout* makeActionsBox()
    {
        QGridLayout *gridAction = new QGridLayout();

        buildComboBox_ = new QComboBox(this);
        buildComboBox_->setObjectName(QString::fromUtf8("buildComboBox_"));
        buildComboBox_->clear();
        buildComboBox_->insertItems(0, QStringList()
                                    << "Data"
                                    << "Mesh"
                                    << "Data & Mesh"
                                    << "Load Dendrogram"
                                    << "Blank Volume"
            );
        gridAction->addWidget(buildComboBox_, 1, 0);

        QPushButton* buildButton = new QPushButton(this);
        buildButton->setText("Build");
        buildButton->setStatusTip("Build Volume");
        buildButton->setCheckable(false);

        om::connect(buildButton, SIGNAL(clicked()),
                    this, SLOT(build()));

        gridAction->addWidget(buildButton, 1, 1);

        return gridAction;
    }
};

} // namespace segmentationInspector
} // namespace om
