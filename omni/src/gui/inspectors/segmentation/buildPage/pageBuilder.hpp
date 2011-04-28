#ifndef PAGE_BUILDER_HPP
#define PAGE_BUILDER_HPP

#include "gui/inspectors/segmentation/buildPage/sourceBox.hpp"
#include "gui/inspectors/volInspector.h"
#include "volume/build/omBuildSegmentation.hpp"

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
        overallContainer->addWidget(makeVolBox());
        overallContainer->addStretch(1);
    }

private Q_SLOTS:
    void build()
    {
        OmBuildSegmentation* bs = new OmBuildSegmentation(sdw_);

        bs->setFileNamesAndPaths(sourceBox_->GetFilesToBuild());

        const QString whatOrHowToBuild = buildComboBox_->currentText();

        if ("Data" == whatOrHowToBuild ){
            bs->BuildImage(om::NON_BLOCKING);

        } else if ( "Mesh" == whatOrHowToBuild ){
            bs->BuildMesh(om::NON_BLOCKING);

        } else if ("Data & Mesh" == whatOrHowToBuild){
            bs->BuildAndMeshSegmentation(om::NON_BLOCKING);

        } else if ("Load Dendrogram" == whatOrHowToBuild){
            bs->loadDendrogram();

        } else if( "Blank Volume" == whatOrHowToBuild ){
            bs->buildBlankVolume();

        } else {
            throw OmArgException("unknown option", whatOrHowToBuild);
        }
    }

private:
    QGroupBox* makeVolBox(){
        return new OmVolInspector(sdw_.GetSegmentation(), this);
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

#endif
