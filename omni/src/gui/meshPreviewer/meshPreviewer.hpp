#pragma once

#include "utility/dataWrappers.h"

#include <QtGui>
#include <boost/shared_ptr.hpp>

class OmViewGroupState;

class MeshPreviewerImpl : public QWidget {
public:
	MeshPreviewerImpl(QWidget* parent,
					  const SegmentationDataWrapper& sdw,
					  OmViewGroupState* vgs);

private:
	SegmentationDataWrapper sdw_;
	OmViewGroupState* vgs_;

	void mesh();

	friend class PreviewButton;
};

class MeshPreviewer : public QDialog {
public:
	MeshPreviewer(QWidget * p,
				  const SegmentationDataWrapper& sdw,
				  OmViewGroupState* vgs)
		: QDialog(p)
	{
		MeshPreviewerImpl* v = new MeshPreviewerImpl(p, sdw, vgs);

		QVBoxLayout* layout = new QVBoxLayout(this);
		layout->addWidget(v);

		setWindowTitle(tr("Mesh Previewer"));
		resize(500, 400);

		show();
 	}
};

