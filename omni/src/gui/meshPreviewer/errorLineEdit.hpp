#ifndef MESH_PREVIEW_ERROR_LINEEDIT_HPP
#define MESH_PREVIEW_ERROR_LINEEDIT_HPP

#include "common/omCommon.h"
#include "gui/widgets/omLineEdit.hpp"
#include "mesh/omMeshParams.hpp"

class MeshPreviewErrorLineEdit : public OmLineEdit {

public:
	MeshPreviewErrorLineEdit(QWidget* widget)
		: OmLineEdit(widget)
	{
		setText(QString::number(OmMeshParams::GetInitialError()));
	}

private:
	void doUpdateFromEditDoneSignel()
	{
		try {
			OmMeshParams::SetInitialError(om::StrToNum<double>(text()));
		} catch (...){
		}
	}
};

#endif

