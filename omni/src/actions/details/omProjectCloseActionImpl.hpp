#ifndef OM_PROJECT_CLOSE_IMPL_HPP
#define OM_PROJECT_CLOSE_IMPL_HPP

#include "common/omCommon.h"
#include "project/omProject.h"

class OmProjectCloseActionImpl {
public:
	OmProjectCloseActionImpl()
	{}

	void Execute()
	{
		//OmProject::Close();
	}

	void Undo()
	{}

	std::string Description() const
	{
		QString lineItem = QString("Closed");
		return lineItem.toStdString();
	}

	QString classNameForLogFile() const {
		return "OmProjectCloseAction";
	}

private:
	template <typename T> friend class OmActionLoggerFSThread;
};

#endif
